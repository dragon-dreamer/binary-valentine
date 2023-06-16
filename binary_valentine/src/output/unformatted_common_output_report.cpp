#include "binary_valentine/output/unformatted_common_output_report.h"

#include <ranges>
#include <utility>
#include <variant>

#include "binary_valentine/core/overloaded.h"

namespace bv::output
{

namespace
{
std::vector<arg_type> safe_copy(std::span<const arg_type> args)
{
	std::vector<arg_type> result;
	result.reserve(args.size());
	for (const auto& arg : args)
	{
		std::visit(core::overloaded{
			[&result] (const auto& value) {
				using type = std::remove_cvref_t<decltype(value)>;
				if constexpr (std::ranges::view<type>)
				{
					result.emplace_back(
						std::basic_string<typename type::value_type>(value));
				}
				else if constexpr (std::is_same_v<type, impl::localizable_string_id>)
				{
					result.emplace_back(impl::owning_localizable_string_id(value));
				}
				else
				{
					result.emplace_back(value);
				}
			}
		}, arg);
	}
	return result;
}
} //namespace

void unformatted_common_output_report::reyield(
	common_report_interface& report_output) const
{
	for (const auto& report : common_reports_)
	{
		forward_log(report_output, report.level,
			report.message_id, report.args, report.arg_names);
	}
}

void unformatted_common_output_report::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	saved_common_report report{
		.level = level,
		.message_id = message_id,
		.args = safe_copy(args),
		.arg_names{arg_names.begin(), arg_names.end()}
	};
	common_reports_.emplace_back(std::move(report));
}

} //namespace bv::output
