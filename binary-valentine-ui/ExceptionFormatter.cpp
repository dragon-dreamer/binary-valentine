#include "ExceptionFormatter.h"

#include <ranges>

#include "binary_valentine/output/format_args_helper.h"

#include "ImmutableContext.h"

namespace bv
{

QString ExceptionFormatter::formatException(std::exception_ptr ptr)
{
    return QString::fromStdString(
        output::format_args_helper::format_utf8_message(
            output::report_level::error,
            "{exception}",
            std::ranges::single_view<const output::arg_type>(ptr),
            std::ranges::single_view<const char*>(output::arg::exception),
            ImmutableContext::getLocalizedResources(),
            ImmutableContext::getImmutableContext().get_exception_formatter()));
}

} // namespace bv
