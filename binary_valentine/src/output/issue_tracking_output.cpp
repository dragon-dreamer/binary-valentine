#include "binary_valentine/output/issue_tracking_output.h"

namespace bv::output
{

void issue_tracking_output::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	status_.analysis_issues.fetch_add(1u, std::memory_order_relaxed);
}

void issue_tracking_output::rule_log_impl(const rule_report_base& info,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	status_.entity_issues.fetch_add(1u, std::memory_order_relaxed);
}

} //namespace bv::output
