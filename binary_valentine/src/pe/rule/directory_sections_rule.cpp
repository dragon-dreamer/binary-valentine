#include "binary_valentine/pe/rule/directory_sections_rule.h"

#include <string_view>
#include <system_error>
#include <type_traits>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/data_directories.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/image_section_search.h"

namespace bv::pe
{

class directory_sections_rule final
	: public core::rule_base<directory_sections_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_directory_sections_rule";
	static constexpr auto reports = output::get_rule_reports<
		directory_sections_rule,
		pe_report::executable_export_directory,
		pe_report::writable_export_directory,
		pe_report::executable_import_directory,
		pe_report::writable_import_directory,
		pe_report::executable_exception_directory,
		pe_report::writable_exception_directory,
		pe_report::executable_relocation_directory,
		pe_report::writable_relocation_directory,
		pe_report::executable_debug_directory,
		pe_report::writable_debug_directory,
		pe_report::executable_tls_directory,
		pe_report::writable_tls_directory,
		pe_report::executable_config_directory,
		pe_report::writable_config_directory,
		pe_report::executable_bound_import_directory,
		pe_report::writable_bound_import_directory,
		pe_report::executable_delay_import_directory,
		pe_report::writable_delay_import_directory,
		pe_report::executable_iat_directory,
		pe_report::writable_entry_point>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image) const
	{
		using enum pe_bliss::core::data_directories::directory_type;
		check_directory_attributes<
			pe_report::executable_export_directory,
			pe_report::writable_export_directory>(reporter, image, exports);
		check_directory_attributes<
			pe_report::executable_import_directory,
			pe_report::writable_import_directory>(reporter, image, imports);
		check_directory_attributes<
			pe_report::executable_exception_directory,
			pe_report::writable_exception_directory>(reporter, image, exception);
		check_directory_attributes<
			pe_report::executable_relocation_directory,
			pe_report::writable_relocation_directory>(reporter, image, basereloc);
		check_directory_attributes<
			pe_report::executable_debug_directory,
			pe_report::writable_debug_directory>(reporter, image, debug);
		check_directory_attributes<
			pe_report::executable_tls_directory,
			pe_report::writable_tls_directory>(reporter, image, tls);
		check_directory_attributes<
			pe_report::executable_config_directory,
			pe_report::writable_config_directory>(reporter, image, config);
		check_directory_attributes<
			pe_report::executable_bound_import_directory,
			pe_report::writable_bound_import_directory>(reporter, image, bound_import);
		check_directory_attributes<
			pe_report::executable_delay_import_directory,
			pe_report::writable_delay_import_directory>(reporter, image, delay_import);
		check_directory_attributes<
			pe_report::executable_iat_directory, void>(reporter, image, iat);
		check_section_attributes<void, pe_report::writable_entry_point>(reporter, image,
			image.get_optional_header().get_raw_address_of_entry_point());
	}

private:
	template<typename ExecutableReport, typename WritableReport, typename Reporter>
	static void check_directory_attributes(Reporter& reporter,
		const pe_bliss::image::image& image,
		pe_bliss::core::data_directories::directory_type dir_type)
	{
		if (!image.get_data_directories().has_nonempty_directory(dir_type))
			return;

		const auto& dir = image.get_data_directories().get_directory(dir_type);
		check_section_attributes<ExecutableReport, WritableReport>(
			reporter, image, dir->virtual_address);
	}

	template<typename ExecutableReport, typename WritableReport, typename Reporter>
	static void check_section_attributes(Reporter& reporter,
		const pe_bliss::image::image& image, pe_bliss::rva_type rva)
	{
		try
		{
			auto [section_it, section_data_it] = pe_bliss::image::section_from_rva(
				image, rva, 1u);
			if (section_it == image.get_section_table().get_section_headers().cend())
				return;

			using enum pe_bliss::section::section_header::characteristics::value;
			if constexpr (!std::is_same_v<ExecutableReport, void>)
			{
				if (section_it->get_characteristics() & mem_execute)
				{
					reporter.template log<ExecutableReport>(
						output::named_arg("section", section_it->get_name()));
				}
			}

			if constexpr (!std::is_same_v<WritableReport, void>)
			{
				if (section_it->get_characteristics() & mem_write)
				{
					reporter.template log<WritableReport>(
						output::named_arg("section", section_it->get_name()));
				}
			}
		}
		catch (const std::system_error&)
		{
		}
	}
};

void directory_sections_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<directory_sections_rule>();
}

} //namespace bv::pe
