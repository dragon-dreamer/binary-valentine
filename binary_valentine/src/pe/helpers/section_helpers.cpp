#include "binary_valentine/pe/helpers/section_helpers.h"

#include <cstddef>

#include "pe_bliss2/core/data_directories.h"
#include "pe_bliss2/image/image_section_search.h"

namespace bv::pe
{

std::size_t section_helpers::count_directories_in_section(
	pe_bliss::section::section_table::header_list::const_iterator section,
	const pe_bliss::image::image& image,
	bool include_entry_point)
{
	std::size_t total_dir_count = 0;
	const auto& data_dirs = image.get_data_directories();
	for (std::size_t dir_index = 0;
		dir_index != data_dirs.get_directories().size();
		++dir_index)
	{
		auto dir_type = static_cast<pe_bliss::core::data_directories::directory_type>(dir_index);
		const auto& dir = data_dirs.get_directory(dir_type);
		if (!dir->virtual_address)
			continue;

		auto [section_it, section_data_it]
			= pe_bliss::image::section_from_rva(image, dir->virtual_address, 1u);
		if (section_it == section)
			++total_dir_count;
	}

	if (!include_entry_point)
		return total_dir_count;

	const auto ep = image.get_optional_header().get_raw_address_of_entry_point();
	if (!ep)
		return total_dir_count;

	auto [section_it, section_data_it] = pe_bliss::image::section_from_rva(image, ep, 1u);
	return (section_it == section) + total_dir_count;
}

} //namespace bv::pe
