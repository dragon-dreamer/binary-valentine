#include "binary_valentine/output/filtering_report_output.h"

#include "binary_valentine/core/rule_selector.h"

namespace bv::output
{

void filtering_report_output::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	if (!selector_.is_enabled(level))
		return;

	forward_log(*target_, level, message_id, args, arg_names);
}

void filtering_report_output::rule_log_impl(const rule_report_base& info,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	if (!selector_.is_enabled(info, args, arg_names, formatter_))
		return;

	forward_rule_log(*target_, info, args, arg_names);
}

} //namespace bv::output
