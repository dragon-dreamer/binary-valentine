#pragma once

#include <string_view>

namespace bv::core
{

class [[nodiscard]] embedded_resource_loader_interface
{
public:
	virtual ~embedded_resource_loader_interface() = default;

	virtual std::string load_file(std::string_view path) const = 0;
};

} //namespace bv::core
