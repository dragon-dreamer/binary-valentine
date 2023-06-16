#include "binary_valentine/pe/generator/pe_generators.h"

#include "binary_valentine/pe/generator/basic_dotnet_info_generator.h"
#include "binary_valentine/pe/generator/basic_pe_info_generator.h"
#include "binary_valentine/pe/generator/debug_directory_generator.h"
#include "binary_valentine/pe/generator/delay_import_directory_generator.h"
#include "binary_valentine/pe/generator/export_directory_generator.h"
#include "binary_valentine/pe/generator/icon_info_generator.h"
#include "binary_valentine/pe/generator/import_based_info_generator.h"
#include "binary_valentine/pe/generator/import_directory_generator.h"
#include "binary_valentine/pe/generator/load_config_directory_generator.h"
#include "binary_valentine/pe/generator/manifest_generator.h"
#include "binary_valentine/pe/generator/pe_image_generator.h"
#include "binary_valentine/pe/generator/resource_directory_generator.h"
#include "binary_valentine/pe/generator/rich_header_generator.h"
#include "binary_valentine/pe/generator/vc_feature_generator.h"
#include "binary_valentine/pe/generator/version_info_generator.h"

namespace bv::pe
{

void pe_generators_factory::add_pe_generators(core::data_generator_list& generators,
	core::async_data_generator_list& async_generators)
{
	basic_dotnet_info_generator_factory::add_generator(generators);
	basic_pe_info_generator_factory::add_generator(generators);
	debug_directory_generator_factory::add_generator(generators);
	pe_image_generator_factory::add_generator(generators);
	load_config_directory_generator_factory::add_generator(generators);
	resource_directory_generator_factory::add_generator(generators);
	version_info_generator_factory::add_generator(generators);
	export_directory_generator_factory::add_generator(generators);
	delay_import_directory_generator_factory::add_generator(generators);
	icon_info_generator_factory::add_generator(generators);
	import_directory_generator_factory::add_generator(generators);
	rich_header_generator_factory::add_generator(generators);
	import_based_info_generator_factory::add_generator(generators);
	vc_feature_generator_factory::add_generator(generators);
	manifest_generator_factory::add_generator(async_generators);
}

} //namespace bv::pe
