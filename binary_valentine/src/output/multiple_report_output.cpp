#include "binary_valentine/output/multiple_report_output.h"

#include "binary_valentine/core/rule_selector.h"

namespace bv::output
{

void multiple_report_output::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	for (const auto& target : targets_)
		forward_log(*target, level, message_id, args, arg_names);
}

void multiple_report_output::rule_log_impl(const rule_report_base& info,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	for (const auto& target : targets_)
		forward_rule_log(*target, info, args, arg_names);
}

} //namespace bv::output
