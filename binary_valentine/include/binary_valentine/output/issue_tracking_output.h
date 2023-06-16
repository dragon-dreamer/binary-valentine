#pragma once

#include <atomic>
#include <cstddef>
#include <memory>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::output
{

struct issue_tracking_status
{
	std::atomic<std::uint32_t> analysis_issues{ 0u };
	std::atomic<std::uint32_t> entity_issues{ 0u };
};

class [[nodiscard]] issue_tracking_output final
	: public entity_report_interface
{
public:
	explicit issue_tracking_output(issue_tracking_status& status) noexcept
		: status_(status)
	{
	}

	virtual void log_impl(report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override;

	virtual void rule_log_impl(const rule_report_base& info,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override;

private:
	issue_tracking_status& status_;
};

} //namespace bv::output
