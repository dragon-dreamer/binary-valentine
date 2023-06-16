#pragma once

#include <cstddef>

namespace bv::core
{

template<std::size_t N>
struct compile_time_string
{
    static constexpr std::size_t length = N - 1;

    constexpr compile_time_string(const char(&s)[N]) noexcept
    {
        for (std::size_t i = 0; i != N; ++i)
            str[i] = s[i];
    }

	char str[N];
};

template<std::size_t N> compile_time_string(const char(&)[N])
    -> compile_time_string<N>;

template<compile_time_string Str1, compile_time_string Str2>
[[nodiscard]]
consteval auto concat() noexcept
{
    char result[Str1.length + Str2.length + 1u];
    char* ptr = result;
    for (std::size_t i = 0; i != Str1.length; ++i)
        *ptr++ = Str1.str[i];
    for (std::size_t i = 0; i != Str2.length; ++i)
        *ptr++ = Str2.str[i];
    *ptr++ = '\0';
    return compile_time_string(result);
}

} //namespace bv::core
