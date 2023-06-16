#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::pe
{

class basic_pe_info_generator_factory final
{
public:
	basic_pe_info_generator_factory() = delete;
	static void add_generator(core::data_generator_list& generators);
};

} //namespace bv::pe
