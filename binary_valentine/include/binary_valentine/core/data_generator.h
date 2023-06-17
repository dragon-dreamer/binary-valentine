#pragma once

#include <utility>

#include "binary_valentine/core/data_generator_helpers.h"
#include "binary_valentine/core/data_generator_interface.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/value_helper.h"

namespace bv::core
{

template<typename Derived>
class [[nodiscard]] data_generator_base
	: public data_generator_interface
	, public data_generator_output_tags<Derived>
{
public:
	using data_generator_interface::data_generator_interface;

	constexpr data_generator_base() noexcept
		: data_generator_interface(Derived::generator_name)
	{
	}

private:
	virtual void generate_data(value_provider_interface& provider) const final
	{
		using generate_type = func_types<&Derived::generate>;

		if constexpr (requires { &Derived::can_generate; })
		{
			using can_generate_type = func_types<&Derived::can_generate>;
			const bool can_generate_values =
				value_helper<typename can_generate_type::dependencies_type>
				::call_with_values(provider, [this](auto&&... values)
			{
				return static_cast<const Derived&>(*this)
					.can_generate(values...);
			});

			if (!can_generate_values)
			{
				typename generate_type::return_type result{};
				impl::set_values_helper<typename generate_type::return_type>::set(
					provider, std::move(result));
				return;
			}
		}

		auto result = value_helper<typename generate_type::dependencies_type>
			::call_with_values(provider, [this](auto&&... values)
		{
			return static_cast<const Derived&>(*this).generate(values...);
		});

		impl::set_values_helper<typename generate_type::return_type>::set(
			provider, std::move(result));
	}
};

} //namespace bv::core
