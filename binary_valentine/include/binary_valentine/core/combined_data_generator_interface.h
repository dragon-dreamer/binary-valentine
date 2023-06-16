#pragma once

#include <span>
#include <string_view>

#include "binary_valentine/core/value_providers_span.h"

namespace bv::core
{

class value_provider_interface;

class [[nodiscard]] combined_data_generator_interface
{
public:
	constexpr explicit combined_data_generator_interface(std::string_view name) noexcept
		: name_(name)
	{
	}

	virtual ~combined_data_generator_interface() = default;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}

	virtual void generate(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values) const = 0;

	[[nodiscard]]
	virtual std::span<const value_tag> get_individual_dependencies() const noexcept = 0;
	[[nodiscard]]
	virtual std::span<const value_tag> get_combined_dependencies() const noexcept = 0;

private:
	std::string_view name_;
};

} //namespace bv::core
