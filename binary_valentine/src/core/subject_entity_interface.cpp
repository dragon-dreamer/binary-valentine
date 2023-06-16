#include "binary_valentine/core/subject_entity_interface.h"

#include <filesystem>

#include "binary_valentine/string/encoding.h"

namespace bv::core
{

std::string subject_entity_interface::name_from_path(const std::filesystem::path& path)
{
	if (path.has_filename())
		return string::to_utf8(path.filename().native());

	return string::to_utf8(path.native());
}

std::string subject_entity_interface::name_from_path(const std::filesystem::path& path,
	const std::filesystem::path& root_path)
{
	if (root_path == path)
		return name_from_path(path);

	std::error_code ec;
	auto relative_path = std::filesystem::relative(path, root_path, ec);
	return string::to_utf8(ec ? path.native() : relative_path.native());
}

} //namespace bv::core
