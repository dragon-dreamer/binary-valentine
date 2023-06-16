#pragma once

#include <string_view>

namespace bv::core
{

class value_provider_interface;

class [[nodiscard]] data_generator_interface
{
public:
	constexpr explicit data_generator_interface(std::string_view name) noexcept
		: name_(name)
	{
	}

	virtual ~data_generator_interface() = default;

	[[nodiscard]]
	std::string_view get_name() const noexcept
	{
		return name_;
	}

	virtual void generate(value_provider_interface& values) const = 0;

private:
	std::string_view name_;
};

} //namespace bv::core
