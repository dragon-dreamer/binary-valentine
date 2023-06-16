#include "binary_valentine/common/xml_loader.h"

#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <new>
#include <stdexcept>

namespace bv::common
{

pugi::xml_document xml_loader::load_xml(std::string_view xml)
{
	pugi::xml_document doc;
	auto result = doc.load_buffer(xml.data(), xml.size());
	if (!result)
		throw std::runtime_error(result.description());

	return doc;
}

pugi::xml_document xml_loader::load_xml(const std::filesystem::path& path)
{
	try
	{
		const auto file_size = static_cast<std::size_t>(std::filesystem::file_size(path));

		auto alloc_func = pugi::get_memory_allocation_function();
		auto dealloc_func = pugi::get_memory_deallocation_function();
		std::unique_ptr<void, decltype(dealloc_func)> mem(alloc_func(file_size), dealloc_func);
		if (!mem)
			throw std::bad_alloc();

		std::ifstream file;
		file.exceptions(std::ios::badbit | std::ios::failbit);
		file.open(path, std::ios::in | std::ios::binary);
		file.read(static_cast<char*>(mem.get()), static_cast<std::streamsize>(file_size));

		pugi::xml_document doc;
		auto result = doc.load_buffer_inplace_own(mem.release(), file_size);
		if (!result)
			throw std::runtime_error(result.description());

		return doc;
	}
	catch (...)
	{
		std::throw_with_nested(std::runtime_error("Unable to load XML " + path.filename().string()));
	}
}

} //namespace bv::common
