#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class rich_data_rule_factory final
{
public:
	rich_data_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::pe
