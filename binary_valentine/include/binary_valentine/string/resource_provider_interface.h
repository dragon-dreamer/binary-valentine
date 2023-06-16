#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "binary_valentine/core/transparent_hash.h"

namespace bv::string
{

class [[nodiscard]] resource_provider_interface
{
public:
	virtual ~resource_provider_interface() = default;

	[[nodiscard]]
	virtual std::string_view get_string(std::string_view key) const = 0;

	[[nodiscard]]
	virtual std::string_view get_language() const noexcept = 0;
};

} //namespace bv::string
