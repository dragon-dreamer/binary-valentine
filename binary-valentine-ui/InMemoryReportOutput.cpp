#include "InMemoryReportOutput.h"

namespace bv
{

void InMemoryReportOutput::log_impl(output::report_level level,
                      std::string_view message_id,
                      std::span<const output::arg_type> args,
                      std::span<const char* const> arg_names)
{
    std::unique_lock lock(mutex_);
    output::in_memory_report_output::log_impl(level, message_id, args, arg_names);
    perLevelCategories_[static_cast<std::size_t>(level)]
        |= (1u << static_cast<std::size_t>(output::report_category::system));
}

void InMemoryReportOutput::rule_log_impl(const output::rule_report_base& info,
                           std::span<const output::arg_type> args,
                           std::span<const char* const> arg_names)
{
    std::unique_lock lock(mutex_);
    output::in_memory_report_output::rule_log_impl(info, args, arg_names);
    perLevelCategories_[static_cast<std::size_t>(info.get_report_level())]
        |= (1u << static_cast<std::size_t>(info.get_report_category()));
}

} //namespace bv
