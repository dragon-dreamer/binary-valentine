#include "binary_valentine/pe/generator/delay_import_directory_generator.h"

#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/delay_import/delay_import_directory_loader.h"

namespace bv::pe
{

class delay_import_directory_generator final
	: public core::data_generator_base<delay_import_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_delay_import_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::delay_import::delay_import_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::delay_import::load(image, {
			.target_directory = pe_bliss::core::data_directories::directory_type::delay_import
		});
		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::delay_import::delay_import_directory_details>(std::move(*result));
	}
};

void delay_import_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<delay_import_directory_generator>();
}

} //namespace bv::pe
