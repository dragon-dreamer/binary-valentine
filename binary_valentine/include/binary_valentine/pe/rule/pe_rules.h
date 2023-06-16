#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class pe_rules_factory final
{
public:
	pe_rules_factory() = delete;
	static void add_pe_rules(core::rule_list& rules,
		core::combined_rule_list& combined_rules,
		core::value_provider_interface& shared_values);
};

} //namespace bv::pe
