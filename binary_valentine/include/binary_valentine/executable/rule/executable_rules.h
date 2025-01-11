#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::executable
{

class executable_rules_factory final
{
public:
	executable_rules_factory() = delete;
	static void add_executable_rules(core::rule_list& rules,
		core::combined_rule_list& combined_rules);
};

} //namespace bv::executable
