#include "binary_valentine/core/file_embedded_resource_loader.h"

#include <stdexcept>

#include "binary_valentine/file/file_loader.h"
#include "binary_valentine/string/encoding.h"

namespace bv::core
{

file_embedded_resource_loader::file_embedded_resource_loader(const std::filesystem::path& resource_path)
	: resource_path_(resource_path)
{
}

std::string file_embedded_resource_loader::load_file(std::string_view path) const
{
	try
	{
		const auto full_path = resource_path_
			/ string::utf8_to<std::filesystem::path::string_type>::convert(path);
		return file::file_loader::load_file(full_path);
	}
	catch (...)
	{
		std::throw_with_nested(std::runtime_error(
			"Unable to load resource file " + std::string(path)));
	}
}

} //namespace bv::core
