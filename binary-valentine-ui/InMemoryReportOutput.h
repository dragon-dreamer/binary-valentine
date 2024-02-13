#pragma once

#include <array>
#include <cstddef>
#include <shared_mutex>

#include "binary_valentine/output/in_memory_report_output.h"

namespace bv
{

class InMemoryReportOutput final
    : public output::in_memory_report_output
{
public:
    using in_memory_report_output::in_memory_report_output;
    using PerLevelCategoriesType = std::array<
        std::size_t, static_cast<std::size_t>(output::report_level::max)>;

public:
    [[nodiscard]]
    std::shared_lock<std::shared_mutex> lockShared() const noexcept
    {
        return std::shared_lock{mutex_};
    }

    [[nodiscard]]
    const PerLevelCategoriesType& getPerLevelCategories() const noexcept {
        return perLevelCategories_;
    }

private:
    virtual void log_impl(output::report_level level,
                          std::string_view message_id,
                          std::span<const output::arg_type> args,
                          std::span<const char* const> arg_names) override;

    virtual void rule_log_impl(const output::rule_report_base& info,
                               std::span<const output::arg_type> args,
                               std::span<const char* const> arg_names) override;

private:
    mutable std::shared_mutex mutex_;
    std::array<std::size_t,
               static_cast<std::size_t>(output::report_level::max)> perLevelCategories_{};
};

} //namespace bv
