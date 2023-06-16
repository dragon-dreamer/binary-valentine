#pragma once

#include <memory>
#include <syncstream>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::string { class resource_provider_interface; }

namespace bv::output
{

class exception_formatter;

class [[nodiscard]] terminal_report_output final
	: public entity_report_interface
{
public:
	explicit terminal_report_output(
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		const string::resource_provider_interface& resource_provider,
		const exception_formatter& formatter) noexcept
		: entity_(entity)
		, resource_provider_(resource_provider)
		, formatter_(formatter)
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
	void output_entity_name(std::osyncstream& stream) const;

private:
	std::shared_ptr<const core::subject_entity_interface> entity_;
	const string::resource_provider_interface& resource_provider_;
	const exception_formatter& formatter_;
};

} //namespace bv::output
