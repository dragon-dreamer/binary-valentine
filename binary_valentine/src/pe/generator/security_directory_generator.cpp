#include "binary_valentine/pe/generator/security_directory_generator.h"

#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/security/security_directory.h"
#include "pe_bliss2/security/security_directory_loader.h"

namespace bv::pe
{

class security_directory_generator final
	: public core::data_generator_base<security_directory_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_security_directory_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::security::security_directory_details> generate(
		const pe_bliss::image::image& image) const
	{
		auto result = pe_bliss::security::load(image);
		if (!result)
			return nullptr;

		auto& entries = result->get_entries();
		if (!entries.empty())
		{
			//Fully load cert data, as some large executable files may not be loaded in full.
			//Authenticode check code requires contiguous buffer to be available.
			auto& cert_buffer = entries[0].get_certificate();
			if (!cert_buffer.is_stateless())
				cert_buffer.copy_referenced_buffer();
		}

		return core::make_value<
			pe_bliss::security::security_directory_details>(std::move(*result));
	}
};

void security_directory_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<security_directory_generator>();
}

} //namespace bv::pe
