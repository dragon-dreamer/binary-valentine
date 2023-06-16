#pragma once

#include <vector>

#include "pe_bliss2/resources/resource_types.h"
#include "pe_bliss2/resources/version_info.h"

namespace bv::pe
{

struct version_info_item
{
	pe_bliss::resources::resource_id_type lang{};
	pe_bliss::resources::version_info_details info;
	pe_bliss::resources::version_info_block_details block;
	bool block_has_errors{};
};

using version_info_list = std::vector<version_info_item>;

[[nodiscard]]
const pe_bliss::resources::version_info_details* find_first_valid_block(
	const version_info_list& version) noexcept;

} //namespace bv::pe
