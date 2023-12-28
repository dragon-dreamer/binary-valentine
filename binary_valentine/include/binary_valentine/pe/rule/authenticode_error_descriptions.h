#pragma once

#include <string_view>
#include <system_error>
#include <unordered_map>

namespace bv::pe
{

[[nodiscard]]
const std::unordered_map<
	std::error_code, std::string_view>& get_authenticode_error_descriptions();

} //namespace bv::pe
