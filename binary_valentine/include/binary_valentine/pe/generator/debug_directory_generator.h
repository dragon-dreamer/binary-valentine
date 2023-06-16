#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::pe
{

class debug_directory_generator_factory final
{
public:
	debug_directory_generator_factory() = delete;
	static void add_generator(core::data_generator_list& generators);
};

} //namespace bv::pe
