#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class ansi_import_rule_factory final
{
public:
	ansi_import_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::pe
