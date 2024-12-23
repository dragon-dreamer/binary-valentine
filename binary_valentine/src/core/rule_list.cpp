#include "binary_valentine/core/rule_list.h"

#include <cstddef>
#include <exception>
#include <optional>
#include <span>
#include <stdexcept>
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
	async_value_provider_interface& provider,
	const value_provider_interface& shared_provider)
{
	std::optional<std::array<const value_interface*, max_dependencies>> rule_prerequisites;

	if (deps.size() > max_dependencies) [[unlikely]]
	{
		throw std::runtime_error(
			"Rule has too many dependencies, increase `max_dependencies`");
	}

	auto begin = rule_prerequisites.emplace().begin();
	for (auto dep : deps)
	{
		if (auto shared_val = shared_provider.try_get(dep.tag))
		{
			*begin++ = *shared_val;
			continue;
		}

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
	async_value_provider_interface& provider,
	const value_provider_interface& shared_provider)
{
	auto deps = co_await get_dependencies(
		rule.get_prerequisite_dependencies(), provider, shared_provider);
	if (!deps || !rule.is_rule_applicable(*deps))
		co_return;

	deps = co_await get_dependencies(
		rule.get_run_dependencies(), provider, shared_provider);
	if (deps)
		rule.run_rule(entity_report, *deps);

	co_return;
}
} //namespace

boost::asio::awaitable<void> enabled_rule_list_base<rule_interface>::run(
	output::entity_report_interface& entity_report,
	output::common_report_interface& common_report,
	async_value_provider_interface& provider,
	const value_provider_interface& shared_provider) const
{
	for (const auto rule_ref : rules_)
	{
		if ((co_await boost::asio::this_coro::cancellation_state).cancelled()
			!= boost::asio::cancellation_type::none)
		{
			break;
		}

		try
		{
			co_await run_rule(rule_ref, entity_report, provider, shared_provider);
		}
		catch (const std::exception&)
		{
			common_report.log(output::report_level::error,
				output::reports::rule_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::rule_name, rule_ref.get().get_name()));
		}
	}
}

boost::asio::awaitable<void> enabled_rule_list_base<combined_rule_interface>::run(
	output::common_report_interface& common_report,
	individual_values_span_type individual_values,
	value_provider_interface& combined_values,
	const value_provider_interface& shared_provider) const
{
	for (const auto rule_ref : rules_)
	{
		if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
			co_return;

		try
		{
			rule_ref.get().run_rule(individual_values, combined_values, shared_provider);
		}
		catch (const std::exception&)
		{
			common_report.log(output::report_level::error,
				output::reports::rule_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::rule_name, rule_ref.get().get_name()));
		}
	}
}

template<typename RuleInterface>
void rule_list_base<RuleInterface>::register_rule(
	std::span<const std::size_t> rule_class_indexes,
	std::shared_ptr<const rule_interface_type>&& rule)
{
	if (!rule)
		return;

	for (auto rule_class_index : rule_class_indexes)
	{
		if (rules_by_type_.size() <= rule_class_index)
			rules_by_type_.resize(rule_class_index + 1);
	}

	for (const auto& report : rule->get_reports())
	{
		if (!registered_reports_.emplace(report.get_string_uid(), report).second)
		{
			throw core_error(core_errc::duplicate_report,
				std::string(report.get_string_uid()));
		}
	}

	for (auto rule_class_index : rule_class_indexes)
		rules_by_type_[rule_class_index].emplace_back(rule);
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
	::get_enabled_rules(std::size_t rule_class_index,
		const rule_selector& selector) const
{
	enabled_rule_list_base<RuleInterface> rules;

	if (rules_by_type_.size() <= rule_class_index)
		return rules;

	for (const auto& rule : rules_by_type_[rule_class_index])
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

template<typename RuleInterface>
enabled_rule_list_base<RuleInterface> rule_list_base<RuleInterface>
	::get_all_rules() const
{
	enabled_rule_list_base<RuleInterface> result;

	std::size_t total_rule_count = 0;
	for (const auto& rules : rules_by_type_)
		total_rule_count += rules.size();

	result.get_rules().reserve(total_rule_count);
	for (const auto& rules : rules_by_type_)
	{
		for (const auto& rule : rules)
			result.get_rules().emplace_back(*rule);
	}

	return result;
}

template class rule_list_base<rule_interface>;
template class rule_list_base<combined_rule_interface>;

} //namespace bv::core
