#include "binary_valentine/pe/rule/pe_file_format_rule.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/image/image.h"

namespace bv::pe
{

class pe_file_format_rule final
	: public core::rule_base<pe_file_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_file_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		pe_file_format_rule,
		pe_report::exe_has_dll_flag,
		pe_report::dll_has_no_dll_flag>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image,
		const basic_pe_info& info) const
	{
		bool has_dll_flag = !!(image.get_file_header().get_characteristics()
			& pe_bliss::core::file_header::characteristics::dll);
		if (info.file_extension == ".exe")
		{
			if (has_dll_flag)
			{
				reporter.template log<pe_report::exe_has_dll_flag>(
					output::named_arg("extension", info.file_extension));
			}
		}
		else if (info.file_extension == ".dll" || info.file_extension == ".ocx")
		{
			if (!has_dll_flag)
			{
				reporter.template log<pe_report::dll_has_no_dll_flag>(
					output::named_arg("extension", info.file_extension));
			}
		}
	}
};

void pe_file_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<pe_file_format_rule>(shared_values);
}

} //namespace bv::pe
