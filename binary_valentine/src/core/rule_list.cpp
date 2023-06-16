#include "binary_valentine/core/rule_list.h"

#include <cstddef>
#include <exception>
#include <optional>
#include <span>
#include <string>

#include "binary_valentine/core/core_error.h"
#include "binary_valentine/core/rule_interface.h"
#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{

namespace
{
constexpr std::size_t max_dependencies = 10;

boost::asio::awaitable<std::optional<std::array<
	const value_interface*, max_dependencies>>> get_dependencies(
	std::span<const optional_dependency> deps,
	async_value_provider_interface& provider)
{
	std::optional<std::array<const value_interface*, max_dependencies>> rule_prerequisites;

	auto begin = rule_prerequisites.emplace().begin();
	for (auto dep : deps)
	{
		auto val = co_await provider.get_async(dep.tag);
		if (dep.required && !val)
		{
			rule_prerequisites.reset();
			co_return rule_prerequisites;
		}
		*begin++ = val;
	}

	co_return rule_prerequisites;
}

boost::asio::awaitable<void> run_rule(const rule_interface& rule,
	output::entity_report_interface& entity_report,
	async_value_provider_interface& provider)
{
	auto deps = co_await get_dependencies(rule.get_prerequisite_dependencies(), provider);
	if (!deps || !rule.is_rule_applicable(*deps))
		co_return;

	deps = co_await get_dependencies(rule.get_run_dependencies(), provider);
	if (deps)
		rule.run(entity_report, *deps);

	co_return;
}
} //namespace

boost::asio::awaitable<void> enabled_rule_list_base<rule_interface>::run(
	output::entity_report_interface& entity_report,
	output::common_report_interface& common_report,
	async_value_provider_interface& provider,
	const std::stop_token& stop_token) const
{
	for (const auto rule_ref : rules_)
	{
		try
		{
			co_await run_rule(rule_ref, entity_report, provider);
		}
		catch (const std::exception&)
		{
			common_report.log(output::report_level::error,
				output::reports::rule_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::rule_name, rule_ref.get().get_name()));
		}

		if (stop_token.stop_requested())
			break;
	}
}

void enabled_rule_list_base<combined_rule_interface>::run(
	output::common_report_interface& common_report,
	individual_values_span_type individual_values,
	value_provider_interface& combined_values,
	const std::stop_token& stop_token) const
{
	for (const auto rule_ref : rules_)
	{
		try
		{
			rule_ref.get().run(individual_values, combined_values);
		}
		catch (const std::exception&)
		{
			common_report.log(output::report_level::error,
				output::reports::rule_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::rule_name, rule_ref.get().get_name()));
		}

		if (stop_token.stop_requested())
			return;
	}
}

template<typename RuleInterface>
void rule_list_base<RuleInterface>::register_rule(rule_class_type rule_class,
	std::unique_ptr<const rule_interface_type>&& rule)
{
	if (!rule)
		return;

	if (rules_by_type_.size() <= rule_class)
		rules_by_type_.resize(rule_class + 1);

	for (const auto& report : rule->get_reports())
	{
		if (!registered_reports_.emplace(report.get_string_uid(), report).second)
		{
			throw core_error(core_errc::duplicate_report,
				std::string(report.get_string_uid()));
		}
	}

	rules_by_type_[rule_class].emplace_back(std::move(rule));
}

template<typename RuleInterface>
output::report_uid rule_list_base<RuleInterface>::get_report_uid(
	std::string_view report_string_uid) const
{
	if (auto it = registered_reports_.find(report_string_uid);
		it != registered_reports_.end())
	{
		return it->second.get_uid();
	}

	return {};
}

template<typename RuleInterface>
enabled_rule_list_base<RuleInterface> rule_list_base<RuleInterface>
	::get_enabled_rules(rule_class_type rule_class,
		const rule_selector& selector) const
{
	enabled_rule_list_base<RuleInterface> rules;

	if (rules_by_type_.size() <= rule_class)
		return rules;

	for (const auto& rule : rules_by_type_[rule_class])
	{
		for (const auto& report : rule->get_reports())
		{
			if (selector.is_enabled(report))
			{
				rules.get_rules().emplace_back(*rule);
				break;
			}
		}
	}

	return rules;
}

template rule_list_base<rule_interface>;
template rule_list_base<combined_rule_interface>;

} //namespace bv::core
