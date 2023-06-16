#include "binary_valentine/output/in_memory_report_output.h"

#include "binary_valentine/string/resource_helper.h"
#include "binary_valentine/string/resource_provider_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/output/format_args_helper.h"

namespace bv::output
{

void in_memory_report_output::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	auto& report = common_reports_.emplace_back(level, message_id,
		format_args_helper::format_utf8_message(
			level,
			resource_provider_.get_string(message_id),
			args,
			arg_names,
			resource_provider_,
			formatter_));

	format_args_helper::format_as_string(
		args, arg_names, formatter_, report.arg_name_to_arg);
}

void in_memory_report_output::rule_log_impl(const rule_report_base& info,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	string::rule_report_resource_helper helper(info, resource_provider_);

	auto& report = rule_reports_.emplace_back(info,
		format_args_helper::format_utf8_message(
			info.get_report_level(),
			helper.get_report_description(),
			args,
			arg_names,
			resource_provider_, formatter_));

	format_args_helper::format_as_string(
		args, arg_names, formatter_, report.arg_name_to_arg);
}

} //namespace bv::output
