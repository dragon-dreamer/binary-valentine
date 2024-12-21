#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class combined_dll_import_names_case_rule_factory final
{
public:
	combined_dll_import_names_case_rule_factory() = delete;
	static void add_rule(core::combined_rule_list& rules);
};

} //namespace bv::pe
