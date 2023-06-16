#pragma once

#include <memory>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{
class rule_selector;
} //namespace bv::core

namespace bv::output
{

class exception_formatter;

class [[nodiscard]] filtering_report_output final
	: public entity_report_interface
{
public:
	explicit filtering_report_output(
		std::shared_ptr<entity_report_interface> target,
		const exception_formatter& formatter,
		const core::rule_selector& selector) noexcept
		: target_(std::move(target))
		, formatter_(formatter)
		, selector_(selector)
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
	std::shared_ptr<entity_report_interface> target_;
	const exception_formatter& formatter_;
	const core::rule_selector& selector_;
};

} //namespace bv::output
