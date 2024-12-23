#include "binary_valentine/analysis/concurrent_analysis_executor.h"

#include <algorithm>
#include <exception>
#include <filesystem>
#include <memory>
#include <system_error>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/core/async_value_provider.h"
#include "binary_valentine/core/inaccessible_subject_entity.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/core/value_cache.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/file/async_target_enumerator.h"
#include "binary_valentine/file/file_entity.h"

namespace bv::analysis
{

namespace
{
std::size_t get_cpu_thread_count(const analysis_plan& plan)
{
	static constexpr std::size_t default_thread_count = 4u;

	if (plan.get_thread_count())
		return plan.get_thread_count();

	auto count = std::thread::hardware_concurrency();
	if (!count)
		return default_thread_count;

	return count;
}

std::uint64_t get_preload_limit(const analysis_plan& plan)
{
	if (auto max_tasks = std::get_if<max_concurrent_tasks>(&plan.get_preload_limit()); max_tasks)
		return max_tasks->value;

	return std::get<max_loaded_targets_size>(plan.get_preload_limit()).value;
}
} //namespace

concurrent_analysis_executor::concurrent_analysis_executor(
	const analysis_plan& plan,
	output::result_report_factory_interface& report_factory,
	const std::shared_ptr<progress::progress_report_interface>& progress_report,
	const immutable_context& global_context,
	const core::value_provider_interface& shared_values)
	: thread::multi_executor_concurrent_io_processing_service<
		concurrent_analysis_executor, impl::loaded_target, std::uint64_t>(
			get_cpu_thread_count(plan), get_preload_limit(plan))
	, plan_(plan)
	, context_(plan, report_factory, global_context, shared_values)
	, common_report_(context_.get_report_factory().get_common_report(
		context_.get_plan().get_global_rule_selector(),
		context_.get_global_context().get_exception_formatter()))
	, progress_report_(progress_report)
	, shared_values_(shared_values)
{
	if (progress_report_)
	{
		target_filtered_callback_ = [progress_report = progress_report_.get()]
			(const std::filesystem::path& path) {
				progress_report->report_progress(std::make_shared<core::inaccessible_subject_entity>(path),
					progress::progress_state::target_skipped_filtered);
			};
	}

	if (plan_.enable_signal_cancellation())
		enable_signal_cancellation();
}

std::uint64_t concurrent_analysis_executor::get_task_weight(
	const io_result_type& result) const noexcept
{
	if (std::get_if<max_concurrent_tasks>(&plan_.get_preload_limit()))
		return 1u;

	return result.file_size;
}

boost::asio::awaitable<void> concurrent_analysis_executor::load_target(
	file::target_entry&& result)
{
	std::shared_ptr<const core::subject_entity_interface> target_entity;
	if (auto* path = std::get_if<std::filesystem::path>(&result.entry); path)
	{
		target_entity = std::make_shared<core::inaccessible_subject_entity>(
			std::move(*path), result.root_path);
	}
	else
	{
		auto& dir_entry = std::get<std::filesystem::directory_entry>(result.entry);
		target_entity = std::make_shared<file::file_entity>(
			std::move(dir_entry), result.root_path);
	}

	const auto create_report = [&](
		const core::rule_class_mask& detected_rule_types) {
			return context_.get_report_factory().get_entity_report(
				target_entity, result.selector,
				context_.get_global_context().get_exception_formatter(),
				detected_rule_types);
		};

	core::rule_class_mask rule_types;

	if (result.ec)
	{
		create_report(rule_types)->log_noexcept(
			output::report_level::critical,
			output::reports::inaccessible_entity,
			output::named_arg(output::arg::exception,
				std::make_exception_ptr(std::system_error(result.ec))),
			output::named_arg(output::arg::entity_name,
				target_entity->get_name()));
		co_return;
	}

	report_progress(target_entity, progress::progress_state::load_started);

	core::value_cache value_cache;

	try
	{
		auto entity_stream = target_entity->get_stream_provider();
		co_await context_.get_global_context().get_detector().detect(
			*target_entity, *entity_stream, value_cache, rule_types);
	}
	catch (...)
	{
		create_report(rule_types)->log_noexcept(output::report_level::error,
			output::reports::entity_load_exception,
			output::current_exception_arg(),
			output::named_arg(output::arg::entity_name,
				target_entity->get_path().native()));
		co_return;
	}

	if (rule_types.empty())
	{
		report_progress(target_entity,
			progress::progress_state::target_skipped_unsupported);
		co_return;
	}

	auto report = create_report(rule_types);
	auto value_provider = std::make_shared<bv::core::async_value_provider>(
		std::move(value_cache),
		context_.get_global_context().get_async_generators(),
		context_.get_global_context().get_generators(),
		*report,
		rule_types);

	std::shared_ptr<const core::subject_entity_interface> entity_ptr_copy;
	if (progress_report_)
		entity_ptr_copy = target_entity;

	auto file_size = target_entity->get_size();
	static_cast<core::value_provider_interface_base&>(*value_provider)
		.set(std::make_unique<
			core::value<std::shared_ptr<const core::subject_entity_interface>>>(
				std::move(target_entity)));

	report_progress(entity_ptr_copy, progress::progress_state::loaded);

	if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
		co_return;

	co_await post_io_result_to_cpu(impl::loaded_target{
		.selector = result.selector,
		.rule_types = std::move(rule_types),
		.value_provider = std::move(value_provider),
		.report = std::move(report),
		.entity = std::move(entity_ptr_copy),
		.file_size = file_size
	});
}

boost::asio::awaitable<void> concurrent_analysis_executor::on_all_tasks_complete()
{
	if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
		co_return;

	try
	{
		report_progress(nullptr, progress::progress_state::combined_analysis_started);
		co_await context_.run_combined_analysis();
	}
	catch (...)
	{
		common_report_->log_noexcept(
			output::report_level::critical,
			output::reports::analysis_exception,
			output::current_exception_arg());
	}
	report_progress(nullptr, progress::progress_state::combined_analysis_completed);
}

boost::asio::awaitable<void> concurrent_analysis_executor::io_task_impl()
{
	try
	{
		co_await file::async_target_enumerator::enumerate(plan_,
			[this](file::target_entry&& entry) -> boost::asio::awaitable<void> {
				co_await load_target(std::move(entry));
			}, target_filtered_callback_);
	}
	catch (...)
	{
		common_report_->log_noexcept(
			output::report_level::critical,
			output::reports::target_enumeration_error,
			output::current_exception_arg());
	}
}

boost::asio::awaitable<void> concurrent_analysis_executor::cpu_task_impl(
	io_result_type result)
{
	report_progress(result.entity, progress::progress_state::analysis_started);

	try
	{
		for (std::size_t rule_class_index : result.rule_types)
		{
			if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
				co_return;

			auto enabled_rules = context_.get_global_context().get_rules()
				.get_enabled_rules(rule_class_index, result.selector);
			co_await enabled_rules.run(*result.report, *result.report,
				*result.value_provider, shared_values_);
		}

		if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
			co_return;

		co_await context_.store_values_for_combined_analysis(
			std::move(result.value_provider),
			result.report, result.rule_types,
			result.selector);
	}
	catch (...)
	{
		result.report->log_noexcept(
			output::report_level::critical,
			output::reports::entity_processing_error,
			output::current_exception_arg());
	}

	report_progress(result.entity, progress::progress_state::analysis_completed);
}

void concurrent_analysis_executor::stop_requested()
{
	common_report_->log_noexcept(
		output::report_level::info,
		output::reports::explicit_stop_requested);
}

void concurrent_analysis_executor::report_progress(
	const std::shared_ptr<const core::subject_entity_interface>& entity,
	progress::progress_state state) noexcept
{
	if (!progress_report_)
		return;

	progress_report_->report_progress(entity, state);
}

} //namespace bv::analysis
