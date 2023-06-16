#include "binary_valentine/pe/generator/export_directory_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/exports/export_directory.h"
#include "pe_bliss2/exports/export_directory_loader.h"

namespace bv::pe
{

class export_directory_generator final
	: public core::data_generator_base<export_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_export_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::exports::export_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::exports::load(image);

		if (!result)
			return nullptr;

		return core::make_value<
			pe_bliss::exports::export_directory_details>(std::move(*result));
	}
};

void export_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<export_directory_generator>();
}

} //namespace bv::pe
