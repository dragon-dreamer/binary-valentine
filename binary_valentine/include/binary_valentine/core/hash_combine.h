#pragma once

#include <cstddef>
#include <functional>

namespace bv::core
{

constexpr void hash_combine(std::size_t& seed, std::size_t hash)
{
    seed ^= hash + 0x9e3779b9u + (seed << 6u) + (seed >> 2u);
}

template<typename T, typename... Other>
constexpr void hash_combine(std::size_t& seed, const T& v, const Other&... other)
{
    hash_combine(seed, std::hash<T>{}(v));
    (hash_combine(seed, other), ...);
}

} //namespace bv::core
