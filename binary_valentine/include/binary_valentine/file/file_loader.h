#pragma once

#include <filesystem>
#include <string>

namespace bv::file
{

class file_loader final
{
public:
	file_loader() = delete;

	[[nodiscard]]
	static std::string load_file(const std::filesystem::path& path);
	[[nodiscard]]
	static std::string load_file(std::string_view path);
};

} //namespace bv::file
