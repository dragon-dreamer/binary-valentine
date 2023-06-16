#pragma once

#include <cstddef>
#include <utility>

namespace bv::core
{

struct transparent_hash final
{
    using is_transparent = void;

    template<typename T>
    std::size_t operator()(const T& value) const
    {
        return std::hash<T>{}(value);
    }
};

} // namespace bv::core
