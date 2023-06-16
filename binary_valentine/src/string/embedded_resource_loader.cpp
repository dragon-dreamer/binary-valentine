#include "binary_valentine/string/embedded_resource_loader.h"

#include "binary_valentine/resource/en.h"
#include "binary_valentine/core/core_error.h"
#include "binary_valentine/string/embedded_resource_provider.h"

namespace bv::string
{

embedded_resource_loader::language_list_type embedded_resource_loader
	::get_supported_languages() const
{
	return { "en" };
}

std::unique_ptr<resource_provider_interface> embedded_resource_loader
	::load(std::string_view language) const
{
	if (language == "en")
	{
		auto result = std::make_unique<embedded_resource_provider>(std::string(language),
			resource::en::strings.begin(),
			resource::en::strings.end());
		if (result->size() != resource::en::strings.size())
			throw core::core_error(core::core_errc::duplicate_localization_ids);
		return result;
	}
	else
	{
		throw core::core_error(core::core_errc::unsupported_language);
	}
}

} //namespace bv::string
