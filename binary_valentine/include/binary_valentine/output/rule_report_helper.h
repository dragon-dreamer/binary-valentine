#pragma once

#include <string>
#include <string_view>
#include <system_error>

#include "binary_valentine/core/compile_time_string.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/internal_report_arg_names.h"

namespace bv::output
{

template<typename Rule>
class [[nodiscard]] rule_report_helper final
{
public:
	explicit constexpr rule_report_helper(
		entity_report_interface& report) noexcept
		: report_(report)
	{
	}

	template<typename Report, typename... NamedArgs>
	void log(const NamedArgs&... args)
	{
		constexpr auto report = get_report<Report>();
		static_assert(report.get_uid().is_valid(), "Unregistered rule description");
		report_.rule_log_noexcept(report, args...);
	}

	template<typename... NamedArgs>
	void log(report_level level,
		std::string_view message_id,
		const NamedArgs&... args) noexcept
	{
		report_.log_noexcept(level, message_id, args...);
	}

	template<typename Report, typename... NamedArgs>
	void log_error_code(std::error_code ec, const NamedArgs&... args)
	{
		constexpr auto report = get_report<Report>();
		static_assert(report.get_uid().is_valid(), "Unregistered rule description");
		try
		{
			throw std::system_error(ec);
		}
		catch (const std::system_error&)
		{
			report_.rule_log_noexcept(report,
				args...,
				output::current_exception_arg());
		}
	}

private:
	template<typename Report>
	static consteval rule_report_base get_report() noexcept
	{
		constexpr rule_report<Rule, Report> rep;
		for (const auto& val : Rule::reports)
		{
			if (val.get_uid() == rep.get_uid())
				return val;
		}

		return {};
	}

private:
	entity_report_interface& report_;
};

} //namespace bv::output
