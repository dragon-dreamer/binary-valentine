#pragma once

#include <string>
#include <string_view>
#include <type_traits>

namespace bv::string
{

namespace impl
{
std::string to_utf8(std::wstring_view str);
} //namespace impl

[[nodiscard]]
std::string to_utf8(std::u16string_view str);

[[nodiscard]]
std::string to_utf8(std::u32string_view str);

[[nodiscard]]
std::string to_utf8(std::u8string_view str);

[[nodiscard]]
constexpr const std::string& to_utf8(const std::string& str) noexcept
{
	return str;
}

[[nodiscard]]
constexpr std::string_view to_utf8(std::string_view str) noexcept
{
	return str;
}

template<typename T>
	requires(std::is_same_v<typename std::remove_cvref_t<T>::value_type, wchar_t>
		&& sizeof(wchar_t) == 2)
[[nodiscard]]
std::string to_utf8(T&& str)
{
	return impl::to_utf8(str);
}

void replace_invalid_characters(std::string& utf8, char32_t replacement = '?');
std::string replace_invalid_characters_copy(std::string_view utf8, char32_t replacement = '?');

template<typename T>
struct utf8_to final {};

template<>
struct utf8_to<std::string> final
{
	utf8_to() = delete;

	[[nodiscard]]
	static constexpr const std::string& convert(const std::string& str)
	{
		return str;
	}
	
	[[nodiscard]]
	static std::string convert(std::string_view str)
	{
		return std::string(str);
	}
};

template<>
struct utf8_to<std::u8string> final
{
	utf8_to() = delete;

	[[nodiscard]]
	static std::u8string convert(std::string_view str);
};

template<>
struct utf8_to<std::u16string> final
{
	utf8_to() = delete;

	[[nodiscard]]
	static std::u16string convert(std::string_view str);
};

template<>
struct utf8_to<std::u32string> final
{
	utf8_to() = delete;

	[[nodiscard]]
	static std::u32string convert(std::string_view str);
};

template<typename T>
	requires(std::is_same_v<typename std::remove_cvref_t<T>::value_type, wchar_t>
		&& sizeof(wchar_t) == 2)
struct utf8_to<T> final
{
	utf8_to() = delete;

	[[nodiscard]]
	static T convert(std::string_view str)
	{
		auto temp = utf8_to<std::u16string>::convert(str);
		std::wstring result;
		result.reserve(temp.size());
		for (auto ch : temp)
			result.push_back(static_cast<wchar_t>(ch));
		return result;
	}
};

} //namespace bv::string
