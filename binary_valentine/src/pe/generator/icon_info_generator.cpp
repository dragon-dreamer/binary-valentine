#include "binary_valentine/pe/generator/icon_info_generator.h"

#include <string_view>
#include <system_error>
#include <utility>
#include <variant>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/pe/data/main_pe_icon.h"
#include "pe_bliss2/resources/icon_cursor_reader.h"
#include "pe_bliss2/resources/resource_directory.h"
#include "pe_bliss2/resources/resource_reader.h"

namespace bv::pe
{

class icon_info_generator final
	: public core::data_generator_base<icon_info_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_icon_info_generator";

	[[nodiscard]]
	core::typed_value_ptr<main_pe_icon> generate(
		const pe_bliss::resources::resource_directory_details& root) const
	{
		auto result = core::make_value<main_pe_icon>();

		try
		{
			pe_bliss::resources::for_each_resource(root,
				pe_bliss::resources::resource_type::icon_group, [&root, &icon = result->get_value()](
					const pe_bliss::resources::resource_directory_entry_details& name_or_id_entry,
					pe_bliss::resources::resource_id_type lang, const buffers::ref_buffer& /* data */) {
				return load_icon(root, name_or_id_entry, lang, icon);
			});
		}
		catch (const std::system_error&)
		{
			result->get_value().icon.reset();
			result->get_value().error = std::current_exception();
		}

		return result;
	}

private:
	static bool load_icon(
		const pe_bliss::resources::resource_directory_details& root,
		const pe_bliss::resources::resource_directory_entry_details& name_or_id_entry,
		pe_bliss::resources::resource_id_type lang,
		main_pe_icon& result)
	{
		const auto& name_or_id = name_or_id_entry.get_name_or_id();
		if (const auto* id = std::get_if<pe_bliss::resources::resource_id_type>(&name_or_id); id)
		{
			result.icon.emplace(
				pe_bliss::resources::icon_group_from_resource_by_lang(root, lang, *id))
				.get_data_list().clear(); //Clear ref data buffers, they are not needed
			result.icon_id_name = *id;
		}
		else if (const auto* name = std::get_if<pe_bliss::packed_utf16_string>(&name_or_id); name)
		{
			result.icon.emplace(
				pe_bliss::resources::icon_group_from_resource_by_lang(root, lang, name->value()))
				.get_data_list().clear(); //Clear ref data buffers, they are not needed
			result.icon_id_name = name->value();
		}

		if (name_or_id_entry.get_directory().get_entries().size() > 1)
			result.multilingual = true;

		return true;
	}
};

void icon_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<icon_info_generator>();
}

} //namespace bv::pe
