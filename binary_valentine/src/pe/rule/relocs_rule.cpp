#include "binary_valentine/pe/rule/relocs_rule.h"

#include <string_view>
#include <system_error>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/data_directories.h"
#include "pe_bliss2/core/file_header.h"
#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/image_section_search.h"

namespace bv::pe
{

class relocs_rule final
	: public core::rule_base<relocs_rule>
{
public:
	using base_type = core::rule_base<relocs_rule>;
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_relocs_rule";
	static constexpr auto reports = output::get_rule_reports<
		relocs_rule,
		pe_report::relocs_stripped,
		pe_report::relocs_section_not_discardable,
		pe_report::dynamic_base_disabled>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code && !info.is_boot
			&& !info.is_pre_win7ce && !info.is_xbox;
	}

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image) const
	{
		if (!(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::dynamic_base))
		{
			reporter.template log<pe_report::dynamic_base_disabled>();
			return;
		}

		if ((image.get_file_header().get_characteristics()
			& pe_bliss::core::file_header::characteristics::relocs_stripped)
			|| !image.get_data_directories().has_reloc())
		{
			reporter.template log<pe_report::relocs_stripped>();
			return;
		}

		const auto& reloc_dir_info = image.get_data_directories().get_directory(
			pe_bliss::core::data_directories::directory_type::basereloc);
		const auto& sections = image.get_section_table();
		try
		{
			auto [section_it, section_data_it] = pe_bliss::image::section_from_rva(
				image, reloc_dir_info->virtual_address, 1u);
			if (section_it == sections.get_section_headers().cend())
			{
				reporter.template log<pe_report::relocs_stripped>();
				return;
			}

			if (!section_it->is_discardable()) {
				reporter.template log<pe_report::relocs_section_not_discardable>(
					output::named_arg("section_name", section_it->get_name()));
			}
		}
		catch (const std::system_error&)
		{
			reporter.template log<pe_report::relocs_stripped>();
		}
	}
};

void relocs_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<relocs_rule>(shared_values);
}

} //namespace bv::pe
