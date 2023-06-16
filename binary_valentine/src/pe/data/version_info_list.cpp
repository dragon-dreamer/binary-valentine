#include "binary_valentine/pe/data/version_info_list.h"

namespace bv::pe
{

const pe_bliss::resources::version_info_details* find_first_valid_block(
	const version_info_list& version) noexcept
{
	if (version.empty())
		return nullptr;

	for (const auto& info : version)
	{
		if (info.block_has_errors || info.info.has_errors())
			continue;

		return &info.info;
	}

	return nullptr;
}

} //namespace bv::pe
