#pragma once

#include <string_view>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/rule_class_mask.h"

namespace bv::core
{

class async_value_provider_interface;

class [[nodiscard]] async_data_generator_interface
{
public:
	constexpr explicit async_data_generator_interface(std::string_view name,
		rule_class_mask supported_rules) noexcept
		: name_(name)
		, supported_rules_(supported_rules)
	{
	}

	virtual ~async_data_generator_interface() = default;

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

	virtual boost::asio::awaitable<void> generate_data(
		async_value_provider_interface& values) const = 0;

private:
	std::string_view name_;
	rule_class_mask supported_rules_;
};

} //namespace bv::core
