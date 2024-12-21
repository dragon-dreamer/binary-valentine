#include "binary_valentine/analysis/immutable_context.h"

#include <memory>

#include "binary_valentine/common/shared_generator/common_generators.h"
#include "binary_valentine/core/embedded_resource_loader_generator.h"
#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/core/value_provider.h"
#include "binary_valentine/executable/executable_extra_rule_detector.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/pe/generator/pe_generators.h"
#include "binary_valentine/pe/pe_format_detector.h"
#include "binary_valentine/pe/rule/pe_rules.h"
#include "binary_valentine/pe/shared_generator/pe_shared_generators.h"

namespace bv::analysis
{

output::report_uid immutable_context::get_report_uid(std::string_view report_string_uid) const
{
	auto uid = rules_.get_report_uid(report_string_uid);
	if (uid.is_valid())
		return uid;

	return combined_rules_.get_report_uid(report_string_uid);
}

immutable_context::immutable_context()
{
	bv::pe::pe_generators_factory::add_pe_generators(generators_, async_generators_);
	bv::pe::pe_rules_factory::add_pe_rules(rules_, combined_rules_);
	detector_.add_file_format_detector(std::make_unique<bv::pe::pe_format_detector>());
	detector_.add_extra_detector(
		std::make_unique<bv::executable::executable_extra_rule_detector>());
}

} //namespace bv::analysis
