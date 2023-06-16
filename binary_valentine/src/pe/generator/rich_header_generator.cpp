#include "binary_valentine/pe/generator/rich_header_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"

#include "buffers/input_buffer_stateful_wrapper.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/rich/rich_header.h"
#include "pe_bliss2/rich/rich_header_loader.h"

namespace bv::pe
{

class rich_header_generator final
	: public core::data_generator_base<rich_header_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_rich_header_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::rich::rich_header> generate(
		const pe_bliss::image::image& image) const
	{
		buffers::input_buffer_stateful_wrapper_ref dos_stub_ref(
			*image.get_dos_stub().data());

		auto result = pe_bliss::rich::load(dos_stub_ref);
		if (!result)
			return nullptr;

		buffers::input_buffer_stateful_wrapper_ref full_headers_ref(
			*image.get_full_headers_buffer().data());
		if (result->get_checksum() != result->calculate_checksum(full_headers_ref))
			return nullptr;

		return core::make_value<pe_bliss::rich::rich_header>(std::move(*result));
	}
};

void rich_header_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<rich_header_generator>();
}

} //namespace bv::pe
