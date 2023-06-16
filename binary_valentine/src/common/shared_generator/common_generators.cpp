#include "binary_valentine/common/shared_generator/common_generators.h"

#include "binary_valentine/common/shared_generator/program_path_generator.h"

namespace bv::common
{

void common_generators_factory::add_common_generators(core::data_generator_list& generators)
{
	program_path_generator_factory::add_generator(generators);
}

} //namespace bv::common
