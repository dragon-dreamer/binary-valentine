#pragma once

#include <cstddef>
#include <string_view>

namespace bv::core
{

template<typename T>
consteval std::string_view type_name() noexcept;

namespace impl
{
template<typename T>
consteval std::string_view func_name() noexcept
{
#if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
#error "Unsupported compiler"
#endif
}

consteval std::size_t prefix_length() noexcept
{
    return func_name<void>().find("void");
}

consteval std::size_t suffix_length()
{
    return func_name<void>().length()
        - prefix_length()
        - std::string_view("void").size();
}

} //namespace impl

template<typename T>
consteval std::string_view type_name() noexcept
{
    constexpr auto func_name = impl::func_name<T>();
    constexpr auto prefix_length = impl::prefix_length();
    constexpr auto type_name_length = func_name.length()
        - prefix_length - impl::suffix_length();
    return func_name.substr(prefix_length, type_name_length);
}

} //namespace bv::core
