#pragma once

#include <array>
#include <string_view>

namespace bv::version
{

constexpr std::string_view tool_name = "Binary Valentine";
constexpr std::string_view tool_description = "Static analysis tool for executable files";
constexpr std::string_view tool_website = "https://binary-valentine.com";
constexpr std::string_view rule_info_uri = "/docs/rule/";
constexpr std::string_view github_url = "https://github.com/dragon-dreamer/binary-valentine";
constexpr std::array tool_version{ 0u, 4u, 0u, 0u };

} //namespace bv::version
