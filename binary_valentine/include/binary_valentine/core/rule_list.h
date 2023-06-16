#pragma once

#include <cstddef>
#include <memory>
#include <functional>
#include <ranges>
#include <stop_token>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/combined_rule_interface.h"
#include "binary_valentine/core/rule_class.h"
#include "binary_valentine/core/rule_interface.h"
#include "binary_valentine/core/value_helper.h"
#include "binary_valentine/output/rule_report.h"

namespace bv::output
{
class entity_report_interface;
class common_report_interface;
} //namespace bv::output

namespace bv::core
{

class async_value_provider_interface;
class rule_selector;
class value_provider_interface;

template<typename T>
class enabled_rule_list_base;

template<>
class [[nodiscard]] enabled_rule_list_base<rule_interface>
{
public:
	using enabled_rule_list_type
		= std::vector<std::reference_wrapper<const rule_interface>>;

public:
	[[nodiscard]]
	boost::asio::awaitable<void> run(
		output::entity_report_interface& entity_report,
		output::common_report_interface& common_report,
		async_value_provider_interface& provider,
		const std::stop_token& stop_token) const;

	[[nodiscard]]
	enabled_rule_list_type& get_rules() noexcept
	{
		return rules_;
	}

	[[nodiscard]]
	const enabled_rule_list_type& get_rules() const noexcept
	{
		return rules_;
	}

private:
	enabled_rule_list_type rules_;
};

template<>
class [[nodiscard]] enabled_rule_list_base<combined_rule_interface>
{
public:
	using enabled_rule_list_type
		= std::vector<std::reference_wrapper<const combined_rule_interface>>;

public:
	void run(output::common_report_interface& common_report,
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		const std::stop_token& stop_token) const;

	[[nodiscard]]
	enabled_rule_list_type& get_rules() noexcept
	{
		return rules_;
	}

	[[nodiscard]]
	const enabled_rule_list_type& get_rules() const noexcept
	{
		return rules_;
	}

private:
	enabled_rule_list_type rules_;
};

using enabled_rule_list = enabled_rule_list_base<rule_interface>;
using enabled_combined_rule_list = enabled_rule_list_base<combined_rule_interface>;

template<typename RuleInterface>
class [[nodiscard]] rule_list_base
{
public:
	using rule_interface_type = RuleInterface;

public:
	template<typename Rule>
	void register_rule(value_provider_interface& shared_values)
	{
		register_rule(static_cast<rule_class_type>(Rule::rule_class),
			make_rule_with_dependencies<Rule>(shared_values));
	}

	template<typename RuleClass>
	[[nodiscard]]
	enabled_rule_list_base<rule_interface_type>
		get_enabled_rules(RuleClass rule_class,
			const rule_selector& selector) const
	{
		return get_enabled_rules(
			static_cast<rule_class_type>(rule_class), selector);
	}

	[[nodiscard]]
	enabled_rule_list_base<rule_interface_type>
		get_enabled_rules(rule_class_type rule_class,
			const rule_selector& selector) const;

	[[nodiscard]]
	output::report_uid get_report_uid(std::string_view report_string_uid) const;

	[[nodiscard]]
	auto get_reports() const
	{
		return registered_reports_ | std::views::values;
	}

private:
	void register_rule(rule_class_type rule_class,
		std::unique_ptr<const rule_interface_type>&& rule);

	template<typename Rule>
	auto make_rule_with_dependencies(value_provider_interface& shared_values)
	{
		return value_helper<typename Rule::constructor_dependencies_type>
			::call_with_values(shared_values, [this](auto&&... values)
		{
			return std::make_unique<Rule>(std::forward<decltype(values)>(values)...);
		});
	}

private:
	std::vector<std::vector<
		std::unique_ptr<const rule_interface_type>>> rules_by_type_;
	std::unordered_map<std::string_view, output::rule_report_base> registered_reports_;
};

using rule_list = rule_list_base<rule_interface>;
using combined_rule_list = rule_list_base<combined_rule_interface>;

} //namespace bv::core
