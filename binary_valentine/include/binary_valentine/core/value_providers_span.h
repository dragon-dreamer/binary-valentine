#pragma once

#include <memory>
#include <span>

#include "binary_valentine/core/value_provider_interface.h"

namespace bv::core
{

using individual_values_span_type
	= std::span<const std::shared_ptr<value_provider_interface_base>>;

} //namespace bv::core
