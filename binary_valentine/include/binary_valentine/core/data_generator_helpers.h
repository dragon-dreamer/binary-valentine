#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <tuple>
#include <utility>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_helper.h"

namespace bv::core
{

class value_provider_interface;

namespace impl
{

template<typename... T>
struct generated_types_list {};

template<typename T>
struct set_values_helper {};

template<typename T>
struct set_values_helper<std::unique_ptr<T>>
{
	using generated_types = generated_types_list<
		typename T::value_type>;

	static void set(value_provider_interface_base& provider,
		std::unique_ptr<T>&& value)
	{
		provider.set(T::get_tag(), std::move(value));
	}
};

template<typename T>
struct set_values_helper<boost::asio::awaitable<T>>
	: set_values_helper<T>
{
};

template<typename... T>
struct set_values_helper<std::tuple<std::unique_ptr<T>...>>
{
	using generated_types = generated_types_list<
		typename T::value_type...>;

	static void set(value_provider_interface& provider,
		std::tuple<std::unique_ptr<T>...>&& values)
	{
		set(provider, std::move(values), std::index_sequence_for<T...>{});
	}

private:
	template<std::size_t... Index>
	static void set(value_provider_interface& provider,
		std::tuple<std::unique_ptr<T>...>&& values,
		std::index_sequence<Index...>)
	{
		(..., (provider.set(T::get_tag(),
			std::move(std::get<Index>(values)))));
	}
};

} //namespace impl

template<typename Derived>
class data_generator_output_tags
{
public:
	[[nodiscard]]
	static consteval auto tags() noexcept
	{
		using generate_type = func_types<&Derived::generate>;
		return prepare_tags(typename impl::set_values_helper<
			typename generate_type::return_type>::generated_types{});
	}

private:
	template<typename... T>
	static consteval auto prepare_tags(impl::generated_types_list<T...>) noexcept
	{
		std::array<value_tag, sizeof...(T)> result{};
		std::size_t i = 0;
		(..., add_tag<T>(result, i));
		return result;
	}

	template<typename T, typename Array>
	static constexpr void add_tag(Array& arr, std::size_t& index) noexcept
	{
		arr[index] = value<T>::get_tag();
		++index;
	}
};

} //namespace bv::core
