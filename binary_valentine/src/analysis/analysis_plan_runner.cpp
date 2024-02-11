#include "binary_valentine/analysis/analysis_plan_runner.h"

#include <cassert>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/analysis/concurrent_analysis_executor.h"
#include "binary_valentine/core/overloaded.h"
#include "binary_valentine/output/configurable_result_report_factory.h"
#include "binary_valentine/output/format/output_format_executor.h"
#include "binary_valentine/output/format/output_format_interface.h"
#include "binary_valentine/output/format/sarif_output_format.h"
#include "binary_valentine/output/format/text_output_format.h"
#include "binary_valentine/output/in_memory_report_creator_type.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/terminal_output_creator.h"
#include "binary_valentine/output/in_memory_output_creator.h"
#include "binary_valentine/output/issue_tracking_output.h"
#include "binary_valentine/output/realtime_report_creator_type.h"
#include "binary_valentine/string/embedded_resource_loader.h"
#include "binary_valentine/string/encoding.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::analysis
{

struct analysis_plan_runner::impl
{
	explicit impl(analysis_plan&& plan, const immutable_context& global_context,
		const string::resource_provider_interface& resources)
		: plan(std::move(plan))
		, resources(resources)
		, issue_tracking_output(
			std::make_shared<output::issue_tracking_output>(issue_tracking_status))
		, global_context(global_context)
	{
	}

	analysis_plan plan;
	output::issue_tracking_status issue_tracking_status;
	const string::resource_provider_interface& resources;
	std::shared_ptr<output::issue_tracking_output> issue_tracking_output;
	std::optional<output::configurable_result_report_factory> report_factory;
	const immutable_context& global_context;
	std::optional<bv::analysis::concurrent_analysis_executor> executor;
};

namespace
{
output::configurable_result_report_factory& init_result_report_factory(
	const string::resource_provider_interface& resources,
	const std::vector<result_report_type>& output_reports,
	const std::vector<output::realtime_report_creator_type>& additional_realtime_reports,
	const output::in_memory_report_creator_type& custom_in_memory_report_creator,
	const std::shared_ptr<output::issue_tracking_output>& issue_tracking_output,
	std::optional<output::configurable_result_report_factory>& factory)
{
	bool use_in_memory_report = !!custom_in_memory_report_creator;
	bool use_terminal_report = false;
	for (const auto& output_report : output_reports)
	{
		std::visit(core::overloaded{
			[&use_terminal_report](result_report_terminal)
				{ use_terminal_report = true; },
			[&use_in_memory_report](result_report_in_memory)
				{ use_in_memory_report = true; },
			[&use_in_memory_report](const result_report_file&)
				{ use_in_memory_report = true; }
		}, output_report);
	}

	std::vector<output::realtime_report_creator_type> creators;
	creators.emplace_back([&issue_tracking_output](
		const std::shared_ptr<const bv::core::subject_entity_interface>&,
		const output::exception_formatter&,
		const string::resource_provider_interface&) {
		return issue_tracking_output;
	});

	if (use_terminal_report)
		creators.emplace_back(output::terminal_output_creator{});

	creators.insert(creators.end(),
		additional_realtime_reports.begin(), additional_realtime_reports.end());

	output::in_memory_report_creator_type in_memory_report_creator;
	if (use_in_memory_report)
	{
		in_memory_report_creator = custom_in_memory_report_creator
			? custom_in_memory_report_creator
			: output::in_memory_output_creator{};
	}
	return factory.emplace(
		resources, std::move(creators), std::move(in_memory_report_creator));
}

void save_report_to_file(const result_report_file& file,
	output::format::analysis_state& state,
	const string::resource_provider_interface& resources,
	const std::filesystem::path& root_path,
	output::format::output_format_executor& saver,
	output::common_report_interface* error_log)
{
	try
	{
		std::shared_ptr<output::format::output_format_interface> output;
		auto path_copy(file.get_path());
		if (path_copy.is_relative())
			path_copy = root_path / path_copy;
		switch (file.get_type())
		{
		case result_report_file_type::text:
			output = std::make_shared<
				output::format::text_output_format>(resources,
					std::move(path_copy));
			break;
		case result_report_file_type::sarif:
			output = std::make_shared<
				output::format::sarif_output_format>(resources,
					std::move(path_copy));
			break;
		default:
			assert(false);
			return;
		}

		saver.save_to(output, state);
	}
	catch (const std::exception&)
	{
		if (!error_log)
			throw;

		error_log->log_noexcept(output::report_level::critical,
			output::reports::writing_report_error,
			output::named_arg("path", file.get_path()),
			output::current_exception_arg());

		++state.analysis_issues;
	}
}
} //namespace

analysis_plan_runner::analysis_plan_runner(analysis_plan&& plan,
	const immutable_context& global_context,
	const string::resource_provider_interface& resources)
	: impl_(std::make_shared<impl>(std::move(plan), global_context, resources))
{
}

void analysis_plan_runner::start()
{
	auto& report_factory = init_result_report_factory(
		impl_->resources, impl_->plan.get_result_reports(),
		impl_->plan.get_realtime_report_creators(),
		impl_->plan.get_custom_in_memory_report_creator(),
		impl_->issue_tracking_output, impl_->report_factory);

	const auto& global_report = impl_->global_context.get_global_report();
	if (!global_report.empty())
	{
		global_report.reyield(*report_factory.get_common_report(
			{}, impl_->global_context.get_exception_formatter()));
	}

	impl_->executor.emplace(impl_->plan, report_factory,
		impl_->plan.get_progress_report(), impl_->global_context);
}

void analysis_plan_runner::join()
{
	impl_->executor.value().join();
}

output::format::analysis_state analysis_plan_runner::write_reports(
	output::common_report_interface* error_log)
{
	auto state = bv::output::format::state_from_time_tracker(
		impl_->executor->get_time_tracker());
	output::format::output_format_executor saver(std::move(
		impl_->report_factory.value()).get_in_memory_reports());

	state.analysis_issues = impl_->issue_tracking_status.analysis_issues;
	state.entity_issues = impl_->issue_tracking_status.entity_issues;

	for (auto& output_report : impl_->plan.get_result_reports())
	{
		std::visit(core::overloaded{
			[](result_report_terminal) {},
			[](result_report_in_memory) {},
			[&saver, &state, error_log, this](const result_report_file& file) {
				save_report_to_file(file, state, impl_->resources,
					impl_->plan.get_root_path(), saver, error_log);
			}
		}, output_report);
	}

	return state;
}

void analysis_plan_runner::interrupt()
{
	impl_->executor.value().stop();
}

} //namespace bv::analysis
