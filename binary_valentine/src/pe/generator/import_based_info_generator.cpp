#include "binary_valentine/pe/generator/import_based_info_generator.h"

#include <memory>
#include <string_view>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/import_based_info.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"
#include "utilities/string.h"

namespace bv::pe
{

class import_based_info_generator final
	: public core::data_generator_base<import_based_info_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_import_based_info_generator";

	[[nodiscard]]
	core::typed_value_ptr<import_based_info> generate(
		const basic_pe_info& info,
		const pe_bliss::imports::import_directory_details* imports,
		const pe_bliss::delay_import::delay_import_directory_details* delay_imports) const
	{
		auto result = core::make_value<import_based_info>();

		if (info.is_boot || !info.has_executable_code)
			return result;

		if (imports)
			check_imports(*imports, result->get_value());
		if (delay_imports)
			check_imports(*delay_imports, result->get_value());

		return result;
	}

private:
	template<typename Directory>
	static void check_imports(const Directory& dir, import_based_info& info)
	{
		std::visit([&info](const auto& libraries) {
			for (const auto& library : libraries)
			{
				if (info.is_dotnet_native
					&& info.is_native_universal_windows_platform
					&& info.is_service)
				{
					return;
				}

				check_library(library, info);
			}
		}, dir.get_list());
	}

	template<typename Library>
	static void check_library(const Library& library, import_based_info& info)
	{
		const auto& library_name = library.get_library_name().value();
		if (!info.is_dotnet_native)
		{
			if (utilities::iequal(library_name, "mrt100.dll")
				|| utilities::iequal(library_name, "mrt100_app.dll"))
			{
				info.is_dotnet_native = true;
				return;
			}
		}

		if (!info.is_native_universal_windows_platform)
		{
			if (utilities::iequal(library_name, "msvcp140_app.dll")
				|| utilities::iequal(library_name, "vcruntime140_app.dll"))
			{
				info.is_native_universal_windows_platform = true;
				return;
			}
		}

		if (!info.is_service
			&& utilities::iequal(library_name, "advapi32.dll"))
		{
			for (const auto& import : library.get_imports())
			{
				using hint_name_type = typename std::remove_reference_t<
					decltype(import)>::hint_name_type;
				const auto* hint_name = std::get_if<hint_name_type>(
					&import.get_import_info());
				if (!hint_name)
					continue;

				const auto& name = hint_name->get_name().value();
				if (name == "StartServiceCtrlDispatcherW"
					|| name == "StartServiceCtrlDispatcherA")
				{
					info.is_service = true;
					return;
				}
			}
		}
	}
};

void import_based_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<import_based_info_generator>();
}

} //namespace bv::pe
