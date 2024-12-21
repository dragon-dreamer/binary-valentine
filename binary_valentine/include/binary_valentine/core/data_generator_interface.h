#pragma once

#include "binary_valentine/core/generic_data_generator_interface.h"

namespace bv::core
{

class value_provider_interface;

class [[nodiscard]] data_generator_interface : public generic_data_generator_interface
{
public:
	using generic_data_generator_interface::generic_data_generator_interface;

	virtual void generate_data(value_provider_interface& values) const = 0;
};

} //namespace bv::core
