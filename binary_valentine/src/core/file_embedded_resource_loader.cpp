#include "binary_valentine/core/file_embedded_resource_loader.h"

#include <fstream>
#include <stdexcept>
#include <string>

namespace bv::core
{

file_embedded_resource_loader::file_embedded_resource_loader(const std::filesystem::path& resource_path)
	: resource_path_(resource_path)
{
}

std::string file_embedded_resource_loader::load_file(std::string_view path) const
{
	const auto full_path = resource_path_ / path;
	try
	{
		const auto file_size = static_cast<std::size_t>(std::filesystem::file_size(full_path));

		std::string result;
		result.resize(file_size);

		std::ifstream file;
		file.exceptions(std::ios::badbit | std::ios::failbit);
		file.open(full_path, std::ios::in | std::ios::binary);
		file.read(result.data(), static_cast<std::streamsize>(file_size));

		return result;
	}
	catch (...)
	{
		std::throw_with_nested(std::runtime_error("Unable to load XML " + full_path.string()));
	}
}

} //namespace bv::core
