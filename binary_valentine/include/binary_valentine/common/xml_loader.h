#pragma once

#include <filesystem>

#define PUGIXML_NO_XPATH
#define PUGIXML_HEADER_ONLY
#include "pugixml/pugixml.hpp"

namespace bv::common
{

class xml_loader final
{
public:
	[[nodiscard]]
	static pugi::xml_document load_xml(const std::filesystem::path& path);
	[[nodiscard]]
	static pugi::xml_document load_xml(std::string_view xml);
};

} //namespace bv::common
