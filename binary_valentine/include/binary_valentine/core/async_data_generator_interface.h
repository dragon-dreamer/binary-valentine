#pragma once

#include <string_view>

#include <boost/asio/awaitable.hpp>

namespace bv::core
{

class async_value_provider_interface;

class [[nodiscard]] async_data_generator_interface
{
public:
	constexpr explicit async_data_generator_interface(std::string_view name) noexcept
		: name_(name)
	{
	}

	virtual ~async_data_generator_interface() = default;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}

	virtual boost::asio::awaitable<void> generate(
		async_value_provider_interface& values) const = 0;

private:
	std::string_view name_;
};

} //namespace bv::core
