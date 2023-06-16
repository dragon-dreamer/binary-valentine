#pragma once

#include <string>
#include <string_view>

namespace bv::string
{

[[nodiscard]]
std::string to_lower_ascii(std::string_view src);
void to_lower_ascii_inplace(std::string& src);

} //namespace bv::string
