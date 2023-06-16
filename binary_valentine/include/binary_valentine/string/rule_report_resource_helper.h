#pragma once

#include <string_view>

namespace bv::output
{
class rule_report_base;
} //namespace bv::output

namespace bv::string
{

class resource_provider_interface;

class [[nodiscard]] rule_report_resource_helper final
{
public:
	explicit rule_report_resource_helper(const output::rule_report_base& report,
		const resource_provider_interface& resource_provider) noexcept
		: report_(report)
		, resource_provider_(resource_provider)
	{
	}

	[[nodiscard]]
	std::string_view get_report_category() const;

	[[nodiscard]]
	std::string_view get_report_level() const;

	[[nodiscard]]
	std::string_view get_report_uid_name() const;

	[[nodiscard]]
	std::string_view get_report_description() const;

private:
	const output::rule_report_base& report_;
	const resource_provider_interface& resource_provider_;
};

} //namespace bv::string
