#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::common
{

class program_path_generator_factory final
{
public:
	program_path_generator_factory() = delete;
	static void add_generator(core::data_generator_list& generators);
};

} //namespace bv::common
