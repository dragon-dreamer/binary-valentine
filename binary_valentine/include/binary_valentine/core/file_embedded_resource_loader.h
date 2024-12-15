#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "binary_valentine/core/embedded_resource_loader_interface.h"

namespace bv::core
{

class [[nodiscard]] file_embedded_resource_loader final
	: public embedded_resource_loader_interface
{
public:
	explicit file_embedded_resource_loader(const std::filesystem::path& resource_path);

	virtual std::string load_file(std::string_view path) const override;

private:
	std::filesystem::path resource_path_;
};

} //namespace bv::core
