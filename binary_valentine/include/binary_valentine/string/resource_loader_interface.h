#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::string
{

class [[nodiscard]] resource_loader_interface
{
public:
	using language_list_type = std::vector<std::string>;

public:
	virtual ~resource_loader_interface() = default;

	[[nodiscard]]
	virtual language_list_type get_supported_languages() const = 0;
	[[nodiscard]]
	virtual std::unique_ptr<resource_provider_interface> load(
		std::string_view language) const = 0;
};

} //namespace bv::string
