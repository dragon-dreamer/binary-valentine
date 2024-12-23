#include "binary_valentine/analysis/analysis_context.h"

#include <cassert>
#include <cstddef>
#include <functional>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/analysis/shared_context.h"
#include "binary_valentine/core/async_value_provider.h"
#include "binary_valentine/core/combined_dependency_helper.h"
#include "binary_valentine/core/combined_value_provider.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/rule_class.h"

namespace bv::analysis
{

namespace
{
constexpr auto max_rule_class = static_cast<std::size_t>(rule_class_type::max);
} //namespace

analysis_context::analysis_context(
	const analysis_plan& plan,
	output::result_report_factory_interface& report_factory,
	const immutable_context& global_context,
	const core::value_provider_interface& shared_values)
	: plan_(plan)
	, report_factory_(report_factory)
	, global_context_(global_context)
	, shared_values_(shared_values)
{
	if (!plan_.do_combined_analysis())
		return;

	common_report_ = report_factory_.get_common_report(
		plan.get_global_rule_selector(),
		global_context.get_exception_formatter());
	assert(!!common_report_);

	combined_rules_per_class_.reserve(max_rule_class);

	for (std::size_t i = 0; i != max_rule_class; ++i)
	{
		auto& class_enabled_rules = combined_rules_per_class_
			.emplace_back(global_context_.get_combined_rules()
				.get_enabled_rules(i, plan.get_global_rule_selector()));
		if (!class_enabled_rules.get_rules().empty())
		{
			has_combined_rules_ = true;
			core::combined_dependency_helper::get_individual_dependencies(
				global_context.get_combined_generators(), class_enabled_rules,
				required_individual_dependencies_, *common_report_);
		}
	}

	if (!has_combined_rules_)
		return;

	combined_context_.value_providers_per_class.resize(
		static_cast<std::size_t>(rule_class_type::max));
}

bool analysis_context::do_combined_analysis() const noexcept
{
	return plan_.do_combined_analysis() && has_combined_rules_;
}

boost::asio::awaitable<void> analysis_context::store_values_for_combined_analysis(
	std::shared_ptr<core::async_value_provider>&& values,
	const std::shared_ptr<output::entity_report_interface>& entity_report,
	const core::rule_class_mask& rule_types,
	std::reference_wrapper<const core::rule_selector> selector)
{
	if (!do_combined_analysis())
		co_return;

	co_await core::combined_dependency_helper::prepare_individual_dependencies(
		required_individual_dependencies_, *values, true);

	auto& provider = static_cast<core::value_provider_interface_base&>(*values);
	provider.set(core::make_value<std::shared_ptr<bv::output::entity_report_interface>>(
			entity_report));
	provider.set(core::make_value<std::reference_wrapper<const core::rule_selector>>(
		selector));

	for (std::size_t rule_class_index : rule_types)
	{
		if (combined_rules_per_class_[rule_class_index].get_rules().empty())
			continue;

		std::shared_ptr<core::value_provider_interface_base> values_copy = values;

		std::lock_guard lock(combined_context_mutex_);
		combined_context_.value_providers_per_class[rule_class_index]
			.emplace_back(std::move(values_copy));
	}
}

boost::asio::awaitable<void> analysis_context::run_combined_analysis(
	std::size_t rule_class_index)
{
	if (!do_combined_analysis())
		co_return;

	const auto& combined_rules = combined_rules_per_class_[rule_class_index];
	if (combined_rules.get_rules().empty())
		co_return;

	auto individual_providers = std::move(
		combined_context_.value_providers_per_class[rule_class_index]);

	core::combined_value_provider combined_provider(
		global_context_.get_combined_generators(),
		std::move(individual_providers),
		*common_report_);

	co_await combined_rules.run(*common_report_,
		combined_provider.get_individual_providers(), combined_provider,
		shared_values_);
}

boost::asio::awaitable<void> analysis_context::run_combined_analysis()
{
	if (!do_combined_analysis())
		co_return;

	for (std::size_t i = 0; i != max_rule_class; ++i)
	{
		if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
			co_return;

		co_await run_combined_analysis(i);
	}
}

} //namespace bv::analysis
