#include "binary_valentine/pe/generator/version_info_generator.h"

#include <memory>
#include <string_view>
#include <utility>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/pe/data/version_info_list.h"

#include "pe_bliss2/resources/resource_directory.h"
#include "pe_bliss2/resources/resource_reader.h"
#include "pe_bliss2/resources/version_info.h"
#include "pe_bliss2/resources/version_info_reader.h"

namespace bv::pe
{

class version_info_generator final
	: public core::data_generator_base<version_info_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_version_info_generator";

	static bool block_has_errors(
		const pe_bliss::resources::version_info_block_details& block)
	{
		if (block.has_errors())
			return true;

		for (const auto& child : block.get_children())
		{
			if (block_has_errors(child))
				return true;
		}

		return false;
	}

	[[nodiscard]]
	core::typed_value_ptr<version_info_list> generate(
		const pe_bliss::resources::resource_directory_details& root) const
	{
		auto result = core::make_value<version_info_list>();

		static constexpr pe_bliss::resources::resource_directory_entry_details::name_or_id_type
			version_info_resource_id(1u);
		pe_bliss::resources::for_each_resource(root, pe_bliss::resources::resource_type::version,
			[&result](
				const pe_bliss::resources::resource_directory_entry_details::name_or_id_type& name_or_id,
				pe_bliss::resources::resource_id_type lang, const buffers::ref_buffer& data)
		{
			if (name_or_id != version_info_resource_id)
				return false;

			buffers::input_buffer_stateful_wrapper ref(data.data());

			//Set copy_value_memory because this info will be used
			//in a combined rule. THis will help free the remaining image memory by the
			//time that rule is called (version info will remain in the
			//value provider, and everything else will be evicted, so we do not
			//want version info to reference the whole image buffer)
			auto info_block = pe_bliss::resources::version_info_from_resource(ref,
				{ .copy_value_memory = true });
			auto& item = result->get_value().emplace_back(lang,
				pe_bliss::resources::get_version_info(info_block));
			item.block = std::move(info_block);
			item.block_has_errors = block_has_errors(item.block);
			return false;
		});

		if (result->get_value().empty())
			return nullptr;

		return result;
	}
};

void version_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<version_info_generator>();
}

} //namespace bv::pe
