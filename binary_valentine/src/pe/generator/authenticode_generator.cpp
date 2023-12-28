#include "binary_valentine/pe/generator/authenticode_generator.h"

#include <exception>
#include <optional>
#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/security/image_authenticode_verifier.h"
#include "pe_bliss2/security/security_directory.h"

namespace bv::pe
{

class authenticode_generator final
	: public core::data_generator_base<authenticode_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_authenticode_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::security::image_authenticode_check_status> generate(
		const pe_bliss::image::image& image,
		const pe_bliss::security::security_directory_details* sec_dir) const
	{
		if (!sec_dir || sec_dir->has_errors())
			return nullptr;

		for (const auto& entry : sec_dir->get_entries())
		{
			if (entry.has_errors())
				return nullptr;
		}

		std::optional<pe_bliss::security::image_authenticode_check_status> result;

		try
		{
			result = pe_bliss::security::verify_authenticode(image, *sec_dir);
			if (!result)
				return nullptr;
		}
		catch (const std::exception&)
		{
			result.emplace().error = std::current_exception();
		}

		return core::make_value<
			pe_bliss::security::image_authenticode_check_status>(std::move(*result));
	}
};

void authenticode_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<authenticode_generator>();
}

} //namespace bv::pe
