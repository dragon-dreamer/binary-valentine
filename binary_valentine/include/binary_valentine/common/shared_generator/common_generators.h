#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::common
{

class common_generators_factory final
{
public:
	common_generators_factory() = delete;
	static void add_common_generators(core::data_generator_list& generators);
};

} //namespace bv::common
