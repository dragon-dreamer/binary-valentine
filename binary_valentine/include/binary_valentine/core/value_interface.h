#pragma once

#include <functional>
#include <memory>
#include <string_view>

namespace bv::core
{

class value_tag final
{
public:
	using tag_type = const void*;

public:
	constexpr explicit value_tag(const std::string_view* ptr) noexcept
		: value_(ptr)
	{
	}

	constexpr value_tag() noexcept = default;

	[[nodiscard]]
	bool operator==(const value_tag&) const = default;

	[[nodiscard]]
	constexpr tag_type value() const noexcept
	{
		return value_;
	}

	[[nodiscard]]
	constexpr std::string_view name() const noexcept
	{
		return value_ ? *value_ : std::string_view{};
	}

private:
	const std::string_view* value_{};
};

class value_interface
{
public:
	virtual ~value_interface() = default;
};

using value_ptr = std::unique_ptr<const value_interface>;

template<typename T>
concept ValueDependency = ((std::is_reference_v<T> || std::is_pointer_v<T>)
	&& std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T>>>);

template<ValueDependency... Dependencies>
struct dependencies {};

} //namespace bv::core

template<>
struct std::hash<bv::core::value_tag>
{
    std::size_t operator()(bv::core::value_tag tag) const noexcept
    {
        return std::hash<bv::core::value_tag::tag_type>{}(tag.value());
    }
};
