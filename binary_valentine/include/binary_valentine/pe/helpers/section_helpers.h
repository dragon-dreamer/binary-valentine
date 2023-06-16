#pragma once

#include <cstddef>

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/section/section_table.h"

namespace bv::pe
{

class section_helpers final
{
public:
	section_helpers() = delete;

	[[nodiscard]]
	static std::size_t count_directories_in_section(
		pe_bliss::section::section_table::header_list::const_iterator section,
		const pe_bliss::image::image& image,
		bool include_entry_point = true);
};

} //namespace bv::pe