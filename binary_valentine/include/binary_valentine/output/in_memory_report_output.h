#pragma once

#include <memory>

#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/output/in_memory_report.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::string { class resource_provider_interface; }

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::output
{

class exception_formatter;

class [[nodiscard]] in_memory_report_output
	: public entity_report_interface
	, public entity_in_memory_report_interface
{
public:
	explicit in_memory_report_output(
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		const string::resource_provider_interface& resource_provider,
		const std::vector<core::rule_class_type>& detected_rule_types,
		const exception_formatter& formatter)
		: entity_(entity)
		, resource_provider_(resource_provider)
		, formatter_(formatter)
		, detected_rule_types_(detected_rule_types)
	{
	}

	virtual void log_impl(report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override;

	virtual void rule_log_impl(const rule_report_base& info,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override;

public:
	[[nodiscard]]
	virtual const saved_rule_reports_type& get_rule_reports() const noexcept override
	{
		return rule_reports_;
	}

	[[nodiscard]]
	virtual const saved_common_reports_type& get_common_reports() const noexcept override
	{
		return common_reports_;
	}

	[[nodiscard]]
	virtual const std::shared_ptr<const core::subject_entity_interface>&
		get_entity() const noexcept override
	{
		return entity_;
	}

	[[nodiscard]]
	const string::resource_provider_interface& get_resource_provider() const noexcept
	{
		return resource_provider_;
	}

	[[nodiscard]]
	virtual std::span<const core::rule_class_type>
		get_detected_rule_types() const noexcept override
	{
		return detected_rule_types_;
	}

private:
	std::shared_ptr<const core::subject_entity_interface> entity_;
	const string::resource_provider_interface& resource_provider_;
	const exception_formatter& formatter_;
	saved_rule_reports_type rule_reports_;
	saved_common_reports_type common_reports_;
	std::vector<core::rule_class_type> detected_rule_types_;
};

} //namespace bv::output
