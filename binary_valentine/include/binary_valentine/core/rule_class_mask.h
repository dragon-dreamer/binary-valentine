#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>

namespace bv::core
{

class rule_class_mask final
{
public:
    struct iterator
    {
    public:
        using difference_type = std::ptrdiff_t;
        using element_type = std::size_t;
        using pointer = element_type*;
        using reference = element_type;

    public:
        constexpr iterator() = default;
        constexpr iterator(std::uint64_t bitmask) noexcept : current_(bitmask) {}

        constexpr reference operator*() const
        {
            return static_cast<std::size_t>(std::countr_zero(current_));
        }

        constexpr auto& operator++()
        {
            std::uint64_t least_significant_bit = current_ & ~(current_ - 1ull);
            current_ ^= least_significant_bit;
            return *this;
        }

        constexpr auto operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator&) const = default;

    private:
        std::uint64_t current_{};
    };

    constexpr auto begin() const { return iterator(bitmask_); }
    constexpr auto end() const { return iterator(); }

public:
    template<typename T>
    constexpr bool is_set(T index) const noexcept
    {
        return (bitmask_ & (1ull << static_cast<std::size_t>(index))) != 0u;
    }

    template<typename T>
    constexpr bool set(T index) noexcept
    {
        return bitmask_ |= (1ull << static_cast<std::size_t>(index));
    }

    constexpr bool empty() const noexcept
    {
        return bitmask_ == 0ull;
    }

private:
    std::uint64_t bitmask_{};
};

} //namespace bv::core
