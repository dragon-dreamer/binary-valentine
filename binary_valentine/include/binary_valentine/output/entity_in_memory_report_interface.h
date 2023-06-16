#pragma once

#include "binary_valentine/output/in_memory_report.h"

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::output
{

class entity_in_memory_report_interface
{
public:
	virtual ~entity_in_memory_report_interface() = default;

	[[nodiscard]]
	virtual const saved_rule_reports_type& get_rule_reports() const noexcept = 0;
	[[nodiscard]]
	virtual const saved_common_reports_type& get_common_reports() const noexcept = 0;
	[[nodiscard]]
	virtual const core::subject_entity_interface* get_entity() const noexcept = 0;
};

} //namespace bv::output
