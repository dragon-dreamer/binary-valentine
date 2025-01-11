#pragma once

#include <span>
#include <string_view>

#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_providers_span.h"
#include "binary_valentine/output/rule_report.h"

namespace bv::core
{

class value_provider_interface;
class shared_value_provider;

class [[nodiscard]] combined_rule_interface
{
public:
	constexpr explicit combined_rule_interface(std::string_view name) noexcept
		: name_(name)
	{
	}

	virtual ~combined_rule_interface() = default;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}

	virtual void run_rule(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		shared_value_provider& shared_values) const = 0;

	[[nodiscard]]
	virtual std::span<const output::rule_report_base> get_reports() const noexcept = 0;

	[[nodiscard]]
	virtual std::span<const optional_dependency> get_individual_dependencies() const noexcept = 0;
	[[nodiscard]]
	virtual std::span<const optional_dependency> get_combined_dependencies() const noexcept = 0;

private:
	std::string_view name_;
};

} //namespace bv::core
