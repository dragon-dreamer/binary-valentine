#include "binary_valentine/string/encoding.h"

#include <string>

#include "utf8.h"
#include "utf8/cpp17.h"

namespace bv::string
{

std::string to_utf8(std::u16string_view str)
{
	return utf8::utf16to8(str);
}

std::string to_utf8(std::u32string_view str)
{
	return utf8::utf32to8(str);
}

std::string to_utf8(std::u8string_view str)
{
	return std::string(reinterpret_cast<const char*>(str.data()), str.size());
}

void replace_invalid_characters(std::string& utf8, char32_t replacement)
{
	auto end = utf8::replace_invalid(utf8.begin(),
		utf8.end(), utf8.begin(), replacement);
	utf8.resize(end - utf8.begin());
}

std::string replace_invalid_characters_copy(std::string_view utf8, char32_t replacement)
{
	std::string copy(utf8);
	replace_invalid_characters(copy, replacement);
	return copy;
}

namespace impl
{
std::string to_utf8(std::wstring_view str)
{
	std::u16string temp;
	temp.reserve(str.size());
	for (auto ch : str)
		temp.push_back(static_cast<char16_t>(ch));
	return string::to_utf8(temp);
}
} //namespace impl

std::u8string utf8_to<std::u8string>::convert(std::string_view str)
{
	std::u8string result;
	result.reserve(str.size());
	for (char ch : str)
		result += static_cast<char8_t>(ch);

	return result;
}

std::u16string utf8_to<std::u16string>::convert(std::string_view str)
{
	return utf8::utf8to16(str);
}

std::u32string utf8_to<std::u32string>::convert(std::string_view str)
{
	return utf8::utf8to32(str);
}

} //namespace bv::string
