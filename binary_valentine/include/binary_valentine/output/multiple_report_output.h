#pragma once

#include <memory>
#include <vector>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{
class rule_selector;
} //namespace bv::core

namespace bv::output
{

class exception_formatter;

class [[nodiscard]] multiple_report_output final
	: public entity_report_interface
{
public:
	explicit multiple_report_output(
		std::vector<std::shared_ptr<entity_report_interface>> targets) noexcept
		: targets_(std::move(targets))
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
	std::vector<std::shared_ptr<entity_report_interface>> targets_;
};

} //namespace bv::output
