#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class cross_resource_version_info_rule_factory final
{
public:
	cross_resource_version_info_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::pe
