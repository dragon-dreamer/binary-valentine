#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::pe
{

class full_winapi_map_generator_factory final
{
public:
	full_winapi_map_generator_factory() = delete;
	static void add_generator(core::data_generator_list& generators);
};

} //namespace bv::pe
