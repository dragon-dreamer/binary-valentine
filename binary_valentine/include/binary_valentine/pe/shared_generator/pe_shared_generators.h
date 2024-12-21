#pragma once

#include "binary_valentine/core/data_generator_list.h"

namespace bv::pe
{

class pe_shared_generators_factory final
{
public:
	pe_shared_generators_factory() = delete;
	static void add_pe_generators(core::async_data_generator_list& generators);
};

} //namespace bv::pe
