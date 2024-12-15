#include "binary_valentine/file/file_loader.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include "binary_valentine/string/encoding.h"

namespace bv::file
{

namespace
{
std::string load_file_impl(const std::filesystem::path& path)
{
	const auto file_size = static_cast<std::size_t>(std::filesystem::file_size(path));

	std::string result;
	result.resize(file_size);

	std::ifstream file;
	file.exceptions(std::ios::badbit | std::ios::failbit);
	file.open(path, std::ios::in | std::ios::binary);
	file.read(result.data(), static_cast<std::streamsize>(file_size));

	return result;
}
} //namespace

std::string file_loader::load_file(std::string_view path)
{
	try
	{
		return load_file_impl(string::utf8_to<
			std::filesystem::path::string_type>::convert(path));
	}
	catch (...)
	{
		std::throw_with_nested(std::runtime_error(
			"Unable to load file " + std::string(path)));
	}
}

std::string file_loader::load_file(const std::filesystem::path& path)
{
	try
	{
		return load_file_impl(path);
	}
	catch (...)
	{
		std::throw_with_nested(std::runtime_error(
			"Unable to load file " + path.string()));
	}
}

} //namespace bv::file
