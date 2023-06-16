#include "binary_valentine/pe/shared_generator/pe_shared_generators.h"

#include "binary_valentine/pe/shared_generator/api_sets_map_generator.h"
#include "binary_valentine/pe/shared_generator/full_winapi_map_generator.h"
#include "binary_valentine/pe/shared_generator/not_recommended_imports_generator.h"

namespace bv::pe
{

void pe_shared_generators_factory::add_pe_generators(core::data_generator_list& generators)
{
	api_sets_map_generator_factory::add_generator(generators);
	full_winapi_map_generator_factory::add_generator(generators);
	not_recommended_imports_generator_factory::add_generator(generators);
}

} //namespace bv::pe
