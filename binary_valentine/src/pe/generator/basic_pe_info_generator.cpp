#include "binary_valentine/pe/generator/basic_pe_info_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/string/case_conv.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/core/file_header.h"
#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/dotnet/dotnet_directory.h"
#include "pe_bliss2/image/image.h"

namespace bv::pe
{

class basic_pe_info_generator final
	: public core::data_generator_base<basic_pe_info_generator>
{
public:
	static constexpr std::string_view generator_name = "basic_pe_info_generator";

	[[nodiscard]]
	core::typed_value_ptr<basic_pe_info> generate(const pe_bliss::image::image& image,
		const pe_bliss::dotnet::cor20_header_details* cor20_header,
		const std::shared_ptr<const core::subject_entity_interface>& entity) const
	{
		auto result = core::make_value<basic_pe_info>();
		auto subsystem = image.get_optional_header().get_subsystem();
		result->get_value().machine_type = image.get_file_header().get_machine_type();
		switch (subsystem)
		{
		case pe_bliss::core::optional_header::subsystem::xbox:
			result->get_value().is_xbox = true;
			break;

		case pe_bliss::core::optional_header::subsystem::native:
			result->get_value().is_driver = true;
			break;

		case pe_bliss::core::optional_header::subsystem::efi_application:
		case pe_bliss::core::optional_header::subsystem::efi_boot_service_driver:
		case pe_bliss::core::optional_header::subsystem::efi_rom:
		case pe_bliss::core::optional_header::subsystem::efi_runtime_driver:
		case pe_bliss::core::optional_header::subsystem::windows_boot_application:
			result->get_value().is_boot = true;
			break;

		case pe_bliss::core::optional_header::subsystem::windows_ce_gui:
			if (image.get_optional_header().get_raw_major_operating_system_version() < 7u)
				result->get_value().is_pre_win7ce = true;
			break;

		default:
			break;
		}

		const auto& data_dirs = image.get_data_directories();
		if (data_dirs.has_resources()
			&& !data_dirs.has_bound_import()
			&& !data_dirs.has_config()
			&& !data_dirs.has_debug()
			&& !data_dirs.has_delay_import()
			&& !data_dirs.has_exception_directory()
			&& !data_dirs.has_exports()
			&& !data_dirs.has_imports()
			&& !data_dirs.has_tls()
			&& !image.get_optional_header().get_raw_address_of_entry_point())
		{
			result->get_value().is_resource_only = true;
		}

		if (cor20_header)
		{
			if (cor20_header->get_flags() & pe_bliss::dotnet::comimage_flags::ilonly)
				result->get_value().is_il_only = true;
			else
				result->get_value().is_mixed = true;

			result->get_value().is_managed = true;
		}

		if (image.get_file_header().get_characteristics()
			& pe_bliss::core::file_header::characteristics::executable_image)
		{
			result->get_value().is_executable = true;
		}

		if (image.get_optional_header().get_magic() == pe_bliss::core::optional_header::magic::pe32)
		{
			if (!cor20_header || (image.get_file_header().get_characteristics()
				& pe_bliss::core::file_header::characteristics::machine_32bit))
			{
				result->get_value().loads_as_32bit = true;
			}
		}

		auto& file_ext_string = result->get_value().file_extension;
		file_ext_string = string::to_utf8(entity->get_path().extension().native());
		string::to_lower_ascii_inplace(file_ext_string);

		auto& file_name = result->get_value().file_name;
		file_name = string::to_utf8(entity->get_path().filename().native());

		if (image.get_optional_header().get_raw_address_of_entry_point()
			|| image.get_data_directories().has_bound_import()
			|| image.get_data_directories().has_delay_import()
			|| image.get_data_directories().has_imports())
		{
			result->get_value().has_executable_code = true;
		}
		else
		{
			for (const auto& section : image.get_section_table().get_section_headers())
			{
				if (section.is_executable())
				{
					result->get_value().has_executable_code = true;
					break;
				}
			}
		}

		return result;
	}
};

void basic_pe_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<basic_pe_info_generator>();
}

} //namespace bv::pe
