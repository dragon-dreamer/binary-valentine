#include "binary_valentine/executable/shared_generator/executable_shared_generators.h"

#include "binary_valentine/executable/shared_generator/electron_version_info_generator.h"

namespace bv::executable
{

void executable_shared_generators_factory::add_executable_generators(
	core::async_data_generator_list& generators)
{
	electron_version_info_generator_factory::add_generator(generators);
}

} //namespace bv::executable
