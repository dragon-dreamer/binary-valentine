#pragma once

#include "binary_valentine/core/rule_list.h"

namespace bv::pe
{

class manifest_format_rule_factory final
{
public:
	manifest_format_rule_factory() = delete;
	static void add_rule(core::rule_list& rules,
		core::value_provider_interface& shared_values);
};

} //namespace bv::pe
