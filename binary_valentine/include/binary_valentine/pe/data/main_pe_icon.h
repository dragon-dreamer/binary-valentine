#pragma once

#include <cstddef>
#include <exception>
#include <optional>
#include <string>
#include <variant>

#include "pe_bliss2/resources/icon_cursor.h"

namespace bv::pe
{

struct main_pe_icon
{
	using icon_name_id_type = std::variant<std::monostate, std::uint32_t, std::u16string>;

	std::optional<pe_bliss::resources::icon_group> icon;
	std::exception_ptr error;
	bool multilingual{};
	icon_name_id_type icon_id_name;
};

} //namespace bv::pe
