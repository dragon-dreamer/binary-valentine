#include "binary_valentine/pe/generator/debug_directory_generator.h"

#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/debug/debug_directory.h"
#include "pe_bliss2/debug/debug_directory_loader.h"

namespace bv::pe
{

class debug_directory_generator final
	: public core::data_generator_base<debug_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_debug_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::debug::debug_directory_list_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::debug::load(image);
		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::debug::debug_directory_list_details>(std::move(*result));
	}
};

void debug_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<debug_directory_generator>();
}

} //namespace bv::pe
