#include "binary_valentine/analysis/shared_context.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/bind_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/deferred.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/parallel_group.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

#include "binary_valentine/common/shared_generator/common_generators.h"
#include "binary_valentine/core/embedded_resource_loader_generator.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/pe/shared_generator/pe_shared_generators.h"

namespace bv::analysis
{

namespace
{
boost::asio::awaitable<void> prepare_shared_dependencies(
	core::async_value_provider& values,
	const core::async_data_generator_list& shared_generators,
	const std::unordered_set<core::value_tag>& needed_shared_dependencies)
{
	if (needed_shared_dependencies.empty())
		co_return;

	std::vector<boost::asio::awaitable<const core::value_interface*>> values_to_produce;
	for (const auto tag : needed_shared_dependencies)
		values_to_produce.emplace_back(values.get_async(tag));

	if (values_to_produce.size() == 1)
	{
		co_await std::move(values_to_produce[0]);
		co_return;
	}

	auto ex = co_await boost::asio::this_coro::executor;
	auto strand = boost::asio::make_strand(ex);

	using running_generator_type = decltype(
		boost::asio::co_spawn(strand, std::move(values_to_produce[0]), boost::asio::deferred));
	
	std::vector<running_generator_type> running_generators;
	running_generators.reserve(values_to_produce.size());
	for (auto& awaitable : values_to_produce)
	{
		running_generators.emplace_back(
			boost::asio::co_spawn(strand, std::move(awaitable), boost::asio::deferred));
	}
	co_await boost::asio::experimental::make_parallel_group(std::move(running_generators))
		.async_wait(
			boost::asio::experimental::wait_for_all(),
			boost::asio::deferred
		);
}

void add_dependencies(
	const core::optional_dependency& dependency,
	const core::async_value_provider& shared_values,
	const core::async_data_generator_list& data_generators,
	std::unordered_set<core::value_tag>& needed_shared_dependencies)
{
	static constexpr core::rule_class_mask all_rule_classes(core::rule_class_mask::max);
	if (shared_values.try_get(dependency.tag) == std::nullopt
		&& data_generators.get(dependency.tag, all_rule_classes) != nullptr)
	{
		needed_shared_dependencies.insert(dependency.tag);
	}
}
} //namespace

shared_context::shared_context(
	std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader)
	: sync_data_generators_(create_sync_shared_generators(std::move(embedded_resource_loader)))
	, shared_generators_(create_shared_generators())
	, shared_values_(shared_generators_, sync_data_generators_, global_reporter_)
{
	embedded_resource_loader_ = static_cast<core::value_provider_interface&>(
		shared_values_.get_sync_provider())
		.get<std::shared_ptr<core::embedded_resource_loader_interface>>()->get();
}

boost::asio::awaitable<void> shared_context::load_shared_dependencies(
	core::enabled_rule_list enabled_rules,
	core::enabled_combined_rule_list enabled_combined_rules)
{
	std::unordered_set<core::value_tag> needed_shared_dependencies;

	const auto add_all_dependencies = [this, &needed_shared_dependencies](
		const auto& dependencies) {
			for (const auto& dependency : dependencies)
			{
				add_dependencies(dependency, shared_values_,
					shared_generators_, needed_shared_dependencies);
			}
		};

	for (const auto& rule : enabled_rules.get_rules())
	{
		add_all_dependencies(rule.get().get_run_dependencies());
		add_all_dependencies(rule.get().get_prerequisite_dependencies());
	}

	for (const auto& rule : enabled_combined_rules.get_rules())
	{
		add_all_dependencies(rule.get().get_individual_dependencies());
		add_all_dependencies(rule.get().get_combined_dependencies());
	}
	
	co_await prepare_shared_dependencies(
		shared_values_, shared_generators_, needed_shared_dependencies);
}

core::async_data_generator_list shared_context::create_shared_generators()
{
	core::async_data_generator_list shared_generators;
	pe::pe_shared_generators_factory::add_pe_generators(shared_generators);
	return shared_generators;
}

core::data_generator_list shared_context::create_sync_shared_generators(
	std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader)
{
	core::data_generator_list shared_generators;
	common::common_generators_factory::add_common_generators(shared_generators);
	core::embedded_resource_loader_generator_factory::add_generator(
		std::move(embedded_resource_loader), shared_generators);
	return shared_generators;
}

} //namespace bv::analysis
