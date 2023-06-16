#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <utility>

#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/core/value_providers_span.h"
#include "binary_valentine/output/rule_report_helper.h"

namespace bv::core
{

//Value from combined provider
template<ValueDependency T>
struct combined_value
{
	using type = T;
};

//Value which should be analyzed by the rule (passes the selector filters)
template<ValueDependency T>
struct filtered_value
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
template<typename T>
struct is_filtered_value<filtered_value<T>> : std::true_type {};

template<typename T>
struct underlying_type
{
	using type = std::remove_cvref_t<std::remove_pointer_t<T>>;
	static constexpr bool is_required = !std::is_pointer_v<T>;
	using result_type = std::conditional_t<is_required,
		const type&,
		const type*
	>;
};

template<typename T>
struct underlying_type<combined_value<T>> : underlying_type<T> {};
template<typename T>
struct underlying_type<filtered_value<T>> : underlying_type<T> {};

template<typename T>
using underlying_type_t = typename underlying_type<T>::type;

template<typename Value, typename Reporter>
struct value_with_reporter
{
	Value value;
	Reporter reporter;
};
} //namespace impl

template<typename... Dependencies>
requires (... && (ValueDependency<Dependencies>
	|| impl::is_combined_value<Dependencies>::value || impl::is_filtered_value<Dependencies>::value))
struct combined_dependencies {};

template<typename T>
class combined_value_helper final {};

template<typename... T>
class combined_value_helper<combined_dependencies<T...>> final
{
private:
	template<typename Type, bool IsCombined, typename Arr>
	static constexpr void fill_dependency_tag(Arr& result, std::size_t& index) noexcept
	{
		if constexpr (impl::is_combined_value<Type>::value == IsCombined)
		{
			result[index++] = optional_dependency{
				.tag = value<impl::underlying_type_t<Type>>::get_tag(),
				.required = impl::underlying_type<Type>::is_required
			};
		}
	}

public:
	template<bool IsCombined>
	[[nodiscard]]
	static consteval auto get_dependencies() noexcept
	{
		std::array<optional_dependency,
			(... + (impl::is_combined_value<T>::value == IsCombined))> result{};
		std::size_t i = 0;
		(..., fill_dependency_tag<T, IsCombined>(result, i));
		return result;
	}

public:
	combined_value_helper() = delete;

	template<typename Rule, typename Func>
	static auto call_with_values(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		Func&& func)
	{
		bool success = true;
		return call_impl(std::forward<Func>(func),
			&success,
			std::index_sequence_for<T...>{},
			fetch_value<Rule, T>(individual_values, combined_values, success)...);
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
			return return_type{};

		return std::forward<Func>(func)(get_value<T, Indexes>(values)...);
	}

	template<typename Rule, typename Type>
	static impl::value_with_reporter<
		typename impl::underlying_type<Type>::result_type,
		output::rule_report_helper<Rule>>
		get_value_with_reporter(const individual_values_span_type::value_type& provider)
	{
		const auto* value = provider->template try_get<impl::underlying_type_t<Type>>();
		if constexpr (impl::underlying_type<Type>::is_required)
		{
			return {
				*value,
				output::rule_report_helper<Rule>(
					**provider->try_get<std::shared_ptr<output::entity_report_interface>>())
			};
		}
		else
		{
			return {
				value,
				output::rule_report_helper<Rule>(
					**provider->try_get<std::shared_ptr<output::entity_report_interface>>())
			};
		}
	}

	template<typename Type>
	static bool has_value(const individual_values_span_type::value_type& provider)
	{
		using nested_type = impl::underlying_type_t<Type>;
		if constexpr (impl::underlying_type<Type>::is_required)
			return !!provider->template try_get<nested_type>();
		else
			return true;
	}

	template<typename Rule, typename Type>
	static decltype(auto) fetch_value(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values,
		bool& success)
	{
		using nested_type = impl::underlying_type_t<Type>;

		if constexpr (!impl::is_combined_value<Type>::value)
		{
			success = true;
			if constexpr (!impl::is_filtered_value<Type>::value)
			{
				return individual_values
					| std::views::filter([](const auto& provider) {
					return has_value<Type>(provider);
				})
					| std::views::transform([](const auto& provider) {
					return get_value_with_reporter<Rule, Type>(provider);
				});
			}
			else
			{
				return individual_values
					| std::views::filter([](const auto& provider) {
					return Rule::applies_to(provider) && has_value<Type>(provider);
				})
					| std::views::transform([](const auto& provider) {
					return get_value_with_reporter<Rule, Type>(provider);
				});
			}
		}
		else
		{
			if (!success)
				return static_cast<decltype(combined_values.get<nested_type>())>(nullptr);

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
