#include "binary_valentine/pe/generator/resource_directory_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/resources/resource_directory.h"
#include "pe_bliss2/resources/resource_directory_loader.h"

namespace bv::pe
{

class resource_directory_generator final
	: public core::data_generator_base<resource_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_resource_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::resources::resource_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::resources::load(image);
		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::resources::resource_directory_details>(std::move(*result));
	}
};

void resource_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<resource_directory_generator>();
}

} //namespace bv::pe
