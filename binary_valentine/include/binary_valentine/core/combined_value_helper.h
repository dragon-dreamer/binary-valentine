#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/core/value_providers_span.h"
#include "binary_valentine/output/rule_report_helper.h"

namespace bv::core
{

class shared_value_provider;

//Value from combined provider
template<ValueDependency T>
struct combined_value
{
	using type = T;
};

enum class value_options
{
	none,
	without_reporter = 1u << 0u
};

template<ValueDependency... T>
struct value_tuple
{
	static_assert(sizeof(T...) > 0u);
};

namespace impl
{
template<typename T>
struct is_value_tuple : std::false_type {};
template<typename... T>
struct is_value_tuple<value_tuple<T...>> : std::true_type {};

template<typename T>
concept ValueDependencyOrTuple = (ValueDependency<T> || is_value_tuple<T>::value);
} //namespace impl

//Value which should be analyzed by the rule (passes the selector filters)
template<impl::ValueDependencyOrTuple T,
	value_options Options = value_options::none>
struct filtered_value
{
	using type = T;
};

template<impl::ValueDependencyOrTuple T,
	value_options Options = value_options::none>
struct unfiltered_value
{
	using type = T;
};

namespace impl
{
template<typename T>
struct is_combined_value : std::false_type {};
template<typename T>
struct is_combined_value<combined_value<T>> : std::true_type {};
template<typename T>
struct is_filtered_value : std::false_type {};
template<typename T, value_options Options>
struct is_filtered_value<filtered_value<T, Options>> : std::true_type {};
template<typename T>
struct is_explicitly_unfiltered_value : std::false_type {};
template<typename T, value_options Options>
struct is_explicitly_unfiltered_value<unfiltered_value<T, Options>>
	: std::true_type {};

template<typename... T>
struct value_type_list {};

template<value_options Options>
struct underlying_type_options
{
	static constexpr bool is_reporter_required
		= (static_cast<std::size_t>(Options)
			& static_cast<std::size_t>(value_options::without_reporter)) == 0;
};

template<typename T, value_options Options>
struct underlying_type_with_options : underlying_type_options<Options>
{
	using type = std::remove_cvref_t<std::remove_pointer_t<T>>;
	using raw_types = value_type_list<T>;
	static constexpr std::size_t value_count = 1u;
	static constexpr bool is_required = !std::is_pointer_v<T>;
	using result_type = std::conditional_t<is_required,
		const type&,
		const type*
	>;
};

template<typename... T, value_options Options>
struct underlying_type_with_options<value_tuple<T...>, Options>
	: underlying_type_options<Options>
{
	using raw_types = value_type_list<T...>;
	static constexpr std::size_t value_count = sizeof...(T);
};

template<typename T>
struct underlying_type : underlying_type_with_options<T, value_options::none> {};

template<typename T>
struct underlying_type<combined_value<T>>
	: underlying_type_with_options<T, value_options::none> {};
template<typename T, value_options Options>
struct underlying_type<filtered_value<T, Options>>
	: underlying_type_with_options<T, Options> {};
template<typename T, value_options Options>
struct underlying_type<unfiltered_value<T, Options>>
	: underlying_type_with_options<T, Options> {};

template<typename T>
using underlying_type_t = typename underlying_type<T>::type;

template<typename T>
struct with_result_type
{
	using result_type = T;
};
} //namespace impl

template<typename... Dependencies>
requires (... && (ValueDependency<Dependencies>
	|| impl::is_combined_value<Dependencies>::value
	|| impl::is_filtered_value<Dependencies>::value
	|| impl::is_explicitly_unfiltered_value<Dependencies>::value))
struct combined_dependencies {};

template<typename T>
class combined_value_helper final {};

template<typename... T>
class combined_value_helper<combined_dependencies<T...>> final
{
private:
	template<typename Type, typename Arr>
	static constexpr void fill_dependency_tag(Arr& result, std::size_t& index) noexcept
	{
		result[index++] = optional_dependency{
			.tag = value<impl::underlying_type_t<Type>>::get_tag(),
			.required = impl::underlying_type<Type>::is_required
		};
	}

	template<bool IsCombinedNeeded, bool IsValueCombined,
		typename... Types, typename Arr>
	static constexpr void fill_dependency_tag(
		impl::value_type_list<Types...> /* types */,
		Arr& result, std::size_t& index) noexcept
	{
		if constexpr (IsCombinedNeeded == IsValueCombined)
			(..., fill_dependency_tag<Types>(result, index));
	}

public:
	template<bool IsCombined>
	[[nodiscard]]
	static consteval auto get_dependencies() noexcept
	{
		std::array<optional_dependency,
			(... + (impl::is_combined_value<T>::value == IsCombined
				? impl::underlying_type<T>::value_count : 0u))> result{};
		std::size_t i = 0;
		(..., fill_dependency_tag<IsCombined, impl::is_combined_value<T>::value>(
			typename impl::underlying_type<T>::raw_types{}, result, i));
		return result;
	}

public:
	combined_value_helper() = delete;

	template<typename Rule, typename Func>
	static auto call_with_values(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		shared_value_provider& shared_values,
		Func&& func)
	{
		bool success = true;
		return call_impl(std::forward<Func>(func),
			&success,
			std::index_sequence_for<T...>{},
			fetch_value<Rule, T>(individual_values, combined_values,
				shared_values, success)...);
	}

private:
	template<typename Type, std::size_t Index, typename Value>
	static decltype(auto) get_value(Value&& value)
	{
		if constexpr (impl::is_combined_value<Type>::value)
		{
			if constexpr (std::is_pointer_v<typename Type::type>)
				return value;
			else
				return *value;
		}
		else
		{
			// view of individual values
			return std::forward<Value>(value);
		}
	}

	template<typename Func, std::size_t... Indexes, typename... Values>
	static auto call_impl(Func&& func, const bool* success,
		std::index_sequence<Indexes...>, Values&&... values)
	{
		using return_type = decltype(
			std::forward<Func>(func)(get_value<T, Indexes>(values)...));

		if (!*success)
		{
			if constexpr (std::is_same_v<return_type, void>)
				return;
			else
				return return_type{};
		}

		return std::forward<Func>(func)(get_value<T, Indexes>(values)...);
	}

	template<typename Rule, typename Type>
	static typename impl::underlying_type<Type>::result_type
		get_value_without_reporter(const individual_values_span_type::value_type& provider)
	{
		const auto* value = provider->template try_get<impl::underlying_type_t<Type>>();
		if constexpr (impl::underlying_type<Type>::is_required)
			return *value;
		else
			return value;
	}

	template<typename Rule, typename... Types>
	static decltype(auto) get_value_without_reporter(impl::value_type_list<Types...> /* types */,
		const individual_values_span_type::value_type& provider)
	{
		if constexpr (sizeof...(Types) == 1u)
		{
			return get_value_without_reporter<Rule, Types...>(provider);
		}
		else
		{
			return std::tuple<typename impl::underlying_type<Types>::result_type...>{
				get_value_without_reporter<Rule, Types>(provider)...};
		}
	}

	template<typename Rule, typename... Types>
	static auto get_value_with_reporter(impl::value_type_list<Types...> /* types */,
		const individual_values_span_type::value_type& provider)
	{
		if constexpr (sizeof...(Types) == 1u)
		{
			return std::tuple<
				typename impl::underlying_type<Types...>::result_type,
				output::rule_report_helper<Rule>>
			{
				get_value_without_reporter<Rule, Types...>(provider),
				output::rule_report_helper<Rule>(
					**provider->try_get<std::shared_ptr<output::entity_report_interface>>())
			};
		}
		else
		{
			return std::tuple<
				typename impl::underlying_type<Types>::result_type...,
				output::rule_report_helper<Rule>>
			{
				get_value_without_reporter<Rule, Types>(provider)...,
				output::rule_report_helper<Rule>(
					**provider->try_get<std::shared_ptr<output::entity_report_interface>>())
			};
		}
	}

	template<typename Rule, typename Traits, typename... Types>
	static decltype(auto) get_value_with_reporter_if_needed(
		impl::value_type_list<Types...> types,
		const individual_values_span_type::value_type& provider)
	{
		if constexpr (Traits::is_reporter_required)
			return get_value_with_reporter<Rule>(types, provider);
		else
			return get_value_without_reporter<Rule>(types, provider);
	}

	template<typename Type>
	static constexpr bool has_value(const individual_values_span_type::value_type& provider)
	{
		using nested_type = impl::underlying_type_t<Type>;
		if constexpr (impl::underlying_type<Type>::is_required)
			return !!provider->template try_get<nested_type>();
		else
			return true;
	}

	template<typename... Types>
	static constexpr bool has_value(impl::value_type_list<Types...> /* types */,
		const individual_values_span_type::value_type& provider)
	{
		return (... && has_value<Types>(provider));
	}

	template<typename Rule, typename Type>
	static decltype(auto) fetch_value(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		shared_value_provider& /* shared_values */,
		bool& success)
	{
		if constexpr (!impl::is_combined_value<Type>::value)
		{
			using value_type_traits = impl::underlying_type<Type>;

			success = true;
			if constexpr (!impl::is_filtered_value<Type>::value)
			{
				return individual_values
					| std::views::filter([](const auto& provider) {
							return has_value(typename value_type_traits::raw_types{}, provider);
						})
					| std::views::transform([](const auto& provider) -> decltype(auto) {
							return get_value_with_reporter_if_needed<Rule, value_type_traits>(
								typename value_type_traits::raw_types{}, provider);
						});
			}
			else
			{
				return individual_values
					| std::views::filter([](const auto& provider) {
							return Rule::applies_to(provider)
								&& has_value(typename value_type_traits::raw_types{}, provider);
						})
					| std::views::transform([](const auto& provider) -> decltype(auto) {
							return get_value_with_reporter_if_needed<Rule, value_type_traits>(
								typename value_type_traits::raw_types{}, provider);
						});
			}
		}
		else
		{
			using nested_type = impl::underlying_type_t<Type>;

			if (!success)
				return static_cast<decltype(combined_values.get<nested_type>())>(nullptr);

			// TODO: `shared_values` are not supported in combined rules

			const auto* value = combined_values.get<nested_type>();
			if constexpr (std::is_pointer_v<typename Type::type>)
				success = true;
			else
				success = value != nullptr;

			return value;
		}
	}
};

} //namespace bv::core
