#pragma once

#include <utility>

#include "binary_valentine/core/data_generator_helpers.h"
#include "binary_valentine/core/combined_data_generator_interface.h"
#include "binary_valentine/core/combined_value_helper.h"

namespace bv::core
{

template<typename Derived>
class [[nodiscard]] combined_data_generator_base
	: public combined_data_generator_interface
	, public data_generator_output_tags<Derived>
{
public:
	using combined_data_generator_interface::combined_data_generator_interface;

	using dependencies_type = combined_dependencies<>;

private:
	virtual void generate(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values) const final
	{
		auto result = combined_value_helper<typename Derived::dependencies_type>
			::call_with_values(individual_values, combined_values,
				[this](auto&&... values)
		{
			return static_cast<const Derived&>(*this).generate(
				std::forward<decltype(values)>(values)...);
		});

		impl::set_values_helper<decltype(result)>::set(
			combined_values, std::move(result));
	}
	
	[[nodiscard]]
	virtual std::span<const value_tag> get_individual_dependencies() const noexcept final
	{
		static constexpr auto deps = combined_value_helper<typename Derived::dependencies_type>
			::template get_dependencies<false>();
		return deps;
	}

	[[nodiscard]]
	virtual std::span<const value_tag> get_combined_dependencies() const noexcept final
	{
		static constexpr auto deps = combined_value_helper<typename Derived::dependencies_type>
			::template get_dependencies<true>();
		return deps;
	}
};

} //namespace bv::core
