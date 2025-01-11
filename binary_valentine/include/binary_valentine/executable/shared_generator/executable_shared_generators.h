#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::executable
{

class executable_shared_generators_factory final
{
public:
	executable_shared_generators_factory() = delete;
	static void add_executable_generators(core::async_data_generator_list& generators);
};

} //namespace bv::executable
