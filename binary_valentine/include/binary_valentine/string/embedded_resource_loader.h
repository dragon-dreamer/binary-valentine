#pragma once

#include <memory>

#include "binary_valentine/string/resource_loader_interface.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::string
{

class [[nodiscard]] embedded_resource_loader final
	: public resource_loader_interface
{
public:
	virtual language_list_type get_supported_languages() const override;
	virtual std::unique_ptr<resource_provider_interface> load(
		std::string_view language) const override;
};

} //namespace bv::string
