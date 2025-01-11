#pragma once

#include <utility>

#include "binary_valentine/core/data_generator_helpers.h"
#include "binary_valentine/core/async_data_generator_interface.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/value_helper.h"

namespace bv::core
{

template<typename Derived>
class [[nodiscard]] async_data_generator_base
	: public async_data_generator_interface
	, public data_generator_output_tags<Derived>
{
public:
	using async_data_generator_interface::async_data_generator_interface;

	static constexpr std::monostate rule_class;

	constexpr async_data_generator_base() noexcept
		: async_data_generator_interface(Derived::generator_name,
			get_supported_rule_class())
	{
	}

private:
	static constexpr rule_class_mask get_supported_rule_class() noexcept
	{
		if constexpr (std::is_same_v<
			std::remove_cvref_t<decltype(Derived::rule_class)>, std::monostate>)
		{
			return rule_class_mask(rule_class_mask::max);
		}
		else
		{
			constexpr auto supported_rule_class
				= static_cast<std::uint64_t>(Derived::rule_class);
			return rule_class_mask(1ull << supported_rule_class);
		}
	}

	virtual std::span<const value_tag>
		get_generated_tags() const noexcept override
	{
		static constexpr auto tags = data_generator_output_tags<Derived>::tags();
		return tags;
	}

	virtual boost::asio::awaitable<void> generate_data(
		async_value_provider_interface& provider) const final
	{
		using generate_type = func_types<&Derived::generate>;

		if constexpr (requires { &Derived::can_generate; })
		{
			using can_generate_type = func_types<&Derived::can_generate>;
			const bool can_generate_values =
				co_await value_helper<typename can_generate_type::dependencies_type>
					::call_with_values(provider, [this](auto&&... values)
				{
					return static_cast<const Derived&>(*this)
						.can_generate(values...);
				});

			if (!can_generate_values)
			{
				typename generate_type::return_type::value_type result{};
				impl::set_values_helper<typename generate_type::return_type>::set(
					provider, std::move(result));
				co_return;
			}
		}

		auto result = co_await value_helper<typename generate_type::dependencies_type>
				::call_with_values(provider, [this](auto&&... values)
			{
				return static_cast<const Derived&>(*this).generate(values...);
			});

		impl::set_values_helper<typename generate_type::return_type>::set(
			provider, std::move(result));
		co_return;
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_prerequisite_dependencies() const noexcept final
	{
		if constexpr (requires { &Derived::can_generate; })
		{
			using can_generate_type = func_types<&Derived::can_generate>;
			static constexpr auto result = value_helper<
				typename can_generate_type::dependencies_type>::get_dependency_array();
			return result;
		}
		else
		{
			return {};
		}
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_run_dependencies() const noexcept final
	{
		using generate_type = func_types<&Derived::generate>;
		static constexpr auto result = value_helper<
			typename generate_type::dependencies_type>::get_dependency_array();
		return result;
	}
};

} //namespace bv::core
