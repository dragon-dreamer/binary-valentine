#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::pe
{

class manifest_generator_factory final
{
public:
	manifest_generator_factory() = delete;
	static void add_generator(core::async_data_generator_list& generators);
};

} //namespace bv::pe
