#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class manifest_assembly_version_rule_factory final
{
public:
	manifest_assembly_version_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::pe
