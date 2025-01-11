#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::executable
{

class electron_version_rule_factory final
{
public:
	electron_version_rule_factory() = delete;
	static void add_rule(core::rule_list& rules);
};

} //namespace bv::executable
