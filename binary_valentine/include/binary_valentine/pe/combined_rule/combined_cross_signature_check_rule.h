#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class combined_cross_signature_check_rule_factory final
{
public:
	combined_cross_signature_check_rule_factory() = delete;
	static void add_rule(core::combined_rule_list& rules,
		core::value_provider_interface& shared_values);
};

} //namespace bv::pe
