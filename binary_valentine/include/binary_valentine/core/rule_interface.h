#pragma once

#include <span>
#include <string_view>

#include "binary_valentine/core/value.h"
#include "binary_valentine/output/rule_report.h"

namespace bv::output { class entity_report_interface; }

namespace bv::core
{

class [[nodiscard]] rule_interface
{
public:
	constexpr explicit rule_interface(std::string_view name) noexcept
		: name_(name)
	{
	}

	virtual ~rule_interface() = default;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}
	
	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_prerequisite_dependencies() const noexcept = 0;
	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_run_dependencies() const noexcept = 0;

	[[nodiscard]]
	virtual bool is_rule_applicable(
		std::span<const value_interface* const> dependencies) const = 0;
	virtual void run(output::entity_report_interface& report,
		std::span<const value_interface* const> dependencies) const = 0;

	[[nodiscard]]
	virtual std::span<const output::rule_report_base>
		get_reports() const noexcept = 0;

private:
	std::string_view name_;
};

} //namespace bv::core
