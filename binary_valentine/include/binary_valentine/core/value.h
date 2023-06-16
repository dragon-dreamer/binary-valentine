#pragma once

#include <memory>
#include <type_traits>
#include <utility>

#include "binary_valentine/core/reflection.h"
#include "binary_valentine/core/value_interface.h"

namespace bv::core
{

template<typename Value>
class [[nodiscard]] value : public value_interface
{
public:
	using value_type = Value;

	template<typename... T>
	constexpr value(T&&... args)
		noexcept(noexcept(Value(std::forward<T>(args)...)))
		: value_(std::forward<T>(args)...)
	{
	}

	[[nodiscard]]
	static consteval value_tag get_tag() noexcept
	{
		return value_tag{ &tag };
	}

	[[nodiscard]]
	const Value& get_value() const noexcept
	{
		return value_;
	}

	[[nodiscard]]
	Value& get_value() noexcept
	{
		return value_;
	}

private:
	static inline constexpr std::string_view tag{type_name<Value>()};

	Value value_;
};

template<typename T>
using typed_value_ptr = std::unique_ptr<value<T>>;

template<typename Value, typename... T>
[[nodiscard]]
auto make_value(T&&... args)
{
	return std::make_unique<value<Value>>(std::forward<T>(args)...);
}

struct [[nodiscard]] optional_dependency final
{
	value_tag tag{};
	bool required{};
};

} //namespace bv::core
