#pragma once

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/generic_data_generator_interface.h"

namespace bv::core
{

class async_value_provider_interface;

class [[nodiscard]] async_data_generator_interface : public generic_data_generator_interface
{
public:
	using generic_data_generator_interface::generic_data_generator_interface;

	virtual boost::asio::awaitable<void> generate_data(
		async_value_provider_interface& values) const = 0;
};

} //namespace bv::core
