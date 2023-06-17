#pragma once

#include <array>
#include <cstddef>
#include <span>
#include <tuple>
#include <type_traits>

#include "binary_valentine/core/async_value_provider_interface.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_provider_interface.h"

namespace bv::core
{

template<typename T>
class value_helper final {};

template<typename... T>
class value_helper<dependencies<T...>> final
{
private:
	template<typename Type>
	using underlying_type = std::remove_cvref_t<std::remove_pointer_t<Type>>;

public:
	value_helper() = delete;

	template<typename... Types>
	static consteval bool is_invocable() noexcept
	{
		return std::is_invocable_v<Types..., T...>;
	}

	[[nodiscard]]
	static consteval auto get_dependency_array() noexcept
	{
		std::array<optional_dependency, sizeof...(T)> result{};
		std::size_t index = 0;
		(..., add_dependency<T>(result, index));
		return result;
	}

	template<typename Values, typename Func>
	static auto call_with_values(Values& provider, Func&& func)
	{
		std::tuple<const underlying_type<T>*...> values;
		if (!fetch_values(provider, values, std::index_sequence_for<T...>{}))
		{
			using return_type = std::invoke_result_t<Func, const T&...>;
			if constexpr (std::is_same_v<return_type, void>)
				return;
			else
				return return_type{};
		}

		return call_impl(std::forward<Func>(func), values,
			std::index_sequence_for<T...>{});
	}

	template<typename Func>
	static auto call_with_values(async_value_provider_interface& provider, Func&& func)
		-> std::invoke_result_t<Func, const T&...>
	{
		std::tuple<const underlying_type<T>*...> values;
		if (!co_await fetch_values(provider, values, std::index_sequence_for<T...>{}))
			co_return typename std::invoke_result_t<Func, const T&...>::value_type{};

		co_return co_await async_call_impl(std::forward<Func>(func), values,
			std::index_sequence_for<T...>{});
	}

private:
	template<typename Dependency, typename Array>
	static constexpr void add_dependency(Array& arr, std::size_t& index) noexcept
	{
		arr[index++] = optional_dependency{
			.tag = value<underlying_type<Dependency>>::get_tag(),
			.required = !std::is_pointer_v<Dependency>
		};
	}

	template<typename Values, typename Tuple, std::size_t... Indexes>
	static bool fetch_values(Values& provider, Tuple& tuple,
		std::index_sequence<Indexes...>)
	{
		return (... && fetch_value<T, Indexes>(provider, tuple));
	}

	template<typename Type, std::size_t Index, typename Tuple>
	static bool fetch_value(value_provider_interface& provider, Tuple& tuple)
	{
		auto& value = std::get<Index>(tuple);
		value = provider.get<underlying_type<Type>>();
		if constexpr (std::is_pointer_v<Type>)
			return true;
		else
			return value != nullptr;
	}

	template<typename Tuple, std::size_t... Indexes>
	static boost::asio::awaitable<bool> fetch_values(
		async_value_provider_interface& provider, Tuple& tuple,
		std::index_sequence<Indexes...>)
	{
		co_return (... && co_await fetch_value<T, Indexes>(provider, tuple));
	}

	template<typename Type, std::size_t Index, typename Tuple>
	static boost::asio::awaitable<bool> fetch_value(
		async_value_provider_interface& provider, Tuple& tuple)
	{
		auto& value = std::get<Index>(tuple);
		value = co_await provider.get_async<underlying_type<Type>>();
		if constexpr (std::is_pointer_v<Type>)
			co_return true;
		else
			co_return value != nullptr;
	}

	template<typename Type, std::size_t Index, typename Tuple>
	static bool fetch_value(std::span<const value_interface* const>& values, Tuple& tuple)
	{
		auto& val = std::get<Index>(tuple);
		const auto* raw_value = values[Index];
		if (!raw_value)
			return std::is_pointer_v<Type>;

		val = &static_cast<const value<underlying_type<Type>>*>(raw_value)->get_value();
		return true;
	}

	template<typename Type, std::size_t Index, typename Tuple>
	static decltype(auto) get_value(const Tuple& values)
	{
		if constexpr (std::is_pointer_v<Type>)
			return std::get<Index>(values);
		else
			return *std::get<Index>(values);
	}

	template<typename Func, typename Tuple, std::size_t... Indexes>
	static auto call_impl(Func&& func, const Tuple& values,
		std::index_sequence<Indexes...>)
	{
		return std::forward<Func>(func)(get_value<T, Indexes>(values)...);
	}

	template<typename Func, typename Tuple, std::size_t... Indexes>
	static auto async_call_impl(Func&& func, const Tuple& values,
		std::index_sequence<Indexes...>)
		-> std::invoke_result_t<Func, const T&...>
	{
		co_return co_await std::forward<Func>(func)(get_value<T, Indexes>(values)...);
	}
};

template<typename Func>
struct func_types_helper {};

template<typename Ret, typename Class, typename... Args>
struct func_types_helper<Ret(Class::*)(Args...) const>
{
	using return_type = Ret;
	using dependencies_type = dependencies<Args...>;
};

template<auto Func>
using func_types = func_types_helper<decltype(Func)>;

template<typename Func>
struct func_types_skip_first_arg_helper {};

template<typename Ret, typename Class, typename Arg, typename... Args>
struct func_types_skip_first_arg_helper<Ret(Class::*)(Arg, Args...) const>
{
	using return_type = Ret;
	using dependencies_type = dependencies<Args...>;
};

template<auto Func>
using func_types_skip_first_arg = func_types_skip_first_arg_helper<decltype(Func)>;

} //namespace bv::core
