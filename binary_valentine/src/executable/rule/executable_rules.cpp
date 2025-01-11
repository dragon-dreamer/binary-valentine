#include "binary_valentine/executable/rule/executable_rules.h"

#include "binary_valentine/executable/rule/electron_version_rule.h"

namespace bv::executable
{

void executable_rules_factory::add_executable_rules(core::rule_list& rules,
	core::combined_rule_list& /* combined_rules */)
{
	electron_version_rule_factory::add_rule(rules);
}

} //namespace bv::executable
