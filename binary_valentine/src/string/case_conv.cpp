#include "binary_valentine/string/case_conv.h"

#include <algorithm>
#include <cctype>

namespace bv::string
{

std::string to_lower_ascii(std::string_view src)
{
	std::string result(src);
	to_lower_ascii_inplace(result);
	return result;
}

void to_lower_ascii_inplace(std::string& src)
{
	std::transform(src.begin(), src.end(), src.begin(), [](char ch) {
		return std::tolower(static_cast<unsigned char>(ch));
	});
}

} //namespace bv::string
