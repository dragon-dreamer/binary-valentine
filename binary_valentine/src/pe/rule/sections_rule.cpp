#include "binary_valentine/pe/rule/sections_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/image/image.h"

namespace bv::pe
{

class sections_rule final
	: public core::rule_base<sections_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_sections_rule";
	static constexpr auto reports = output::get_rule_reports<
		sections_rule,
		pe_report::rwx_section_present,
		pe_report::wx_section_present,
		pe_report::sw_section_present,
		pe_report::writable_rdata_section,
		pe_report::executable_rdata_section>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::image::image& image) const
	{
		for (const auto& section : image.get_section_table().get_section_headers())
		{
			if (section.is_writable() && section.is_executable())
			{
				if (section.is_readable())
				{
					reporter.template log<pe_report::rwx_section_present>(
						output::named_arg("section_name", section.get_name()));
				}
				else
				{
					reporter.template log<pe_report::wx_section_present>(
						output::named_arg("section_name", section.get_name()));
				}
			}

			if (section.is_writable() && section.is_shared())
			{
				reporter.template log<pe_report::sw_section_present>(
					output::named_arg("section_name", section.get_name()));
			}

			if (section.get_name() == ".rdata")
			{
				if (section.is_writable())
					reporter.template log<pe_report::writable_rdata_section>();
				if (section.is_executable())
					reporter.template log<pe_report::executable_rdata_section>();
			}
		}
	}
};

void sections_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<sections_rule>();
}

} //namespace bv::pe
