#include "binary_valentine/pe/generator/load_config_directory_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/load_config/load_config_directory_loader.h"

namespace bv::pe
{

class load_config_directory_generator final
	: public core::data_generator_base<load_config_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_load_config_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::load_config::load_config_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::load_config::load(image, {
			.load_lock_prefix_table = false,
			.load_dynamic_relocation_table = false,
			.load_volatile_metadata = false,
			.load_ehcont_targets = false,
			.load_xfg_type_based_hashes = false
		});

		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::load_config::load_config_directory_details>(std::move(*result));
	}
};

void load_config_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<load_config_directory_generator>();
}

} //namespace bv::pe
