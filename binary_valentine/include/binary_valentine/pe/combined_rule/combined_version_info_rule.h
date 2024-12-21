#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class combined_version_info_rule_factory final
{
public:
	combined_version_info_rule_factory() = delete;
	static void add_rule(core::combined_rule_list& rules);
};

} //namespace bv::pe
