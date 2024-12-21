#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::core { class value_provider_interface; }

namespace bv::pe
{

class version_info_rule_factory final
{
public:
	version_info_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::pe
