#include "binary_valentine/pe/generator/basic_dotnet_info_generator.h"

#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/dotnet/dotnet_directory.h"
#include "pe_bliss2/dotnet/dotnet_directory_loader.h"

namespace bv::pe
{

class basic_dotnet_info_generator final
	: public core::data_generator_base<basic_dotnet_info_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_basic_dotnet_info_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::dotnet::cor20_header_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::dotnet::load(image);
		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::dotnet::cor20_header_details>(std::move(*result));
	}
};

void basic_dotnet_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<basic_dotnet_info_generator>();
}

} //namespace bv::pe
