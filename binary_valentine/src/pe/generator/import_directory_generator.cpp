#include "binary_valentine/pe/generator/import_directory_generator.h"

#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/imports/import_directory.h"
#include "pe_bliss2/imports/import_directory_loader.h"

namespace bv::pe
{

class import_directory_generator final
	: public core::data_generator_base<import_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_import_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::imports::import_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::imports::load(image);
		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::imports::import_directory_details>(std::move(*result));
	}
};

void import_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<import_directory_generator>();
}

} //namespace bv::pe
