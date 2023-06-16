#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class application_manifest_rule_factory final
{
public:
	application_manifest_rule_factory() = delete;
	static void add_rule(core::rule_list& rules,
		core::value_provider_interface& shared_values);
};

} //namespace bv::pe
