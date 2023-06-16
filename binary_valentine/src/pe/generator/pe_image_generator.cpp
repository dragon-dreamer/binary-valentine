#include "binary_valentine/pe/generator/pe_image_generator.h"

#include <array>
#include <memory>
#include <string_view>
#include <tuple>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/image_loader.h"

namespace bv::pe
{

class pe_image_generator final
	: public core::data_generator_base<pe_image_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_image_generator";

	using result_type = std::tuple<
		core::typed_value_ptr<pe_bliss::image::image>,
		core::typed_value_ptr<pe_bliss::error_list>,
		core::typed_value_ptr<std::exception_ptr>
	>;

	[[nodiscard]]
	result_type generate(const std::shared_ptr<buffers::input_buffer_interface>& buf) const
	{
		result_type result{
			core::make_value<pe_bliss::image::image>(),
			core::make_value<pe_bliss::error_list>(),
			core::make_value<std::exception_ptr>()
		};

		auto& [image, warnings, fatal_error] = result;
		pe_bliss::image::image_loader::load(image->get_value(),
			warnings->get_value(), fatal_error->get_value(), buf);

		if (fatal_error->get_value())
		{
			warnings.reset();
			image.reset();
		}
		else
		{
			fatal_error.reset();
		}

		return result;
	}
};

void pe_image_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<pe_image_generator>();
}

} //namespace bv::pe
