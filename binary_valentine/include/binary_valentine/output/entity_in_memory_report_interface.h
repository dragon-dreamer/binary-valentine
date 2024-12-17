#pragma once

#include <memory>

#include "binary_valentine/core/rule_class_mask.h"
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
	virtual const std::shared_ptr<const core::subject_entity_interface>&
		get_entity() const noexcept = 0;
	[[nodiscard]]
	virtual const core::rule_class_mask&
		get_detected_rule_types() const noexcept = 0;
};

} //namespace bv::output
