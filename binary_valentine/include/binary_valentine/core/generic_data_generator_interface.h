#pragma once

#include <span>
#include <string_view>

#include "binary_valentine/core/rule_class_mask.h"
#include "binary_valentine/core/value.h"

namespace bv::core
{

class value_provider_interface;

class [[nodiscard]] generic_data_generator_interface
{
public:
	constexpr explicit generic_data_generator_interface(std::string_view name,
		rule_class_mask supported_rules) noexcept
		: name_(name)
		, supported_rules_(supported_rules)
	{
	}

	virtual ~generic_data_generator_interface() = default;

	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_prerequisite_dependencies() const noexcept = 0;
	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_run_dependencies() const noexcept = 0;
	[[nodiscard]]
	virtual std::span<const value_tag>
		get_generated_tags() const noexcept = 0;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}

	[[nodiscard]]
	bool should_run_on(const rule_class_mask& detected_mask) const noexcept
	{
		return supported_rules_.is_set(detected_mask.value());
	}

private:
	std::string_view name_;
	rule_class_mask supported_rules_;
};

} //namespace bv::core
