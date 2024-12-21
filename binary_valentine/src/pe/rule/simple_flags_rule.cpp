#include "binary_valentine/pe/rule/simple_flags_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/file_header.h"
#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/image/image.h"

namespace bv::pe
{

class simple_flags_rule final
	: public core::rule_base<simple_flags_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_simple_flags_rule";
	static constexpr auto reports = output::get_rule_reports<
		simple_flags_rule,
		pe_report::not_large_address_aware,
		pe_report::not_large_address_aware_no_high_entropy_va,
		pe_report::no_high_entropy_va,
		pe_report::dep_disabled,
		pe_report::aslr_compatibility_mode,
		pe_report::not_terminal_server_aware>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image,
		const basic_pe_info& basic_info) const
	{
		check_high_entropy_aslr(reporter, image, basic_info);
		check_aslr_compatibility_mode(reporter, image, basic_info);
		check_dep(reporter, image, basic_info);
		check_terminal_server_aware(reporter, image, basic_info);
	}

private:
	template<typename Reporter>
	static void check_terminal_server_aware(Reporter& reporter,
		const pe_bliss::image::image& image, const basic_pe_info& basic_info)
	{
		if (!basic_info.is_executable || !basic_info.has_executable_code || basic_info.is_boot)
			return;

		if (!(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::terminal_server_aware))
		{
			reporter.template log<pe_report::not_terminal_server_aware>();
		}
	}

	template<typename Reporter>
	static void check_high_entropy_aslr(Reporter& reporter, const pe_bliss::image::image& image,
		const basic_pe_info& basic_info)
	{
		if (!basic_info.is_executable || basic_info.loads_as_32bit || !basic_info.has_executable_code
			|| basic_info.is_boot)
		{
			return;
		}

		const bool large_address_aware = static_cast<bool>(image.get_file_header().get_characteristics()
			& pe_bliss::core::file_header::characteristics::large_address_aware);
		const bool high_entropy_va = static_cast<bool>(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::high_entropy_va);

		if (!large_address_aware && !high_entropy_va)
			reporter.template log<pe_report::not_large_address_aware_no_high_entropy_va>();
		else if (!high_entropy_va)
			reporter.template log<pe_report::no_high_entropy_va>();
		else if (!large_address_aware)
			reporter.template log<pe_report::not_large_address_aware>();
	}

	template<typename Reporter>
	static void check_aslr_compatibility_mode(Reporter& reporter,
		const pe_bliss::image::image& image, const basic_pe_info& basic_info)
	{
		if (!basic_info.is_executable || basic_info.loads_as_32bit
			|| !basic_info.has_executable_code || basic_info.is_il_only
			|| basic_info.is_boot)
		{
			return;
		}

		if (image.get_optional_header().get_raw_image_base() < 0xffffffffull)
		{
			reporter.template log<pe_report::aslr_compatibility_mode>(
				output::named_arg("image_base",
					image.get_optional_header().get_raw_image_base()));
		}
	}

	template<typename Reporter>
	static void check_dep(Reporter& reporter, const pe_bliss::image::image& image,
		const basic_pe_info& basic_info)
	{
		if (basic_info.is_xbox || !basic_info.is_executable
			|| !basic_info.has_executable_code || basic_info.is_pre_win7ce || basic_info.is_boot)
		{
			return;
		}

		if (!(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::nx_compat))
		{
			reporter.template log<pe_report::dep_disabled>();
		}
	}
};

void simple_flags_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<simple_flags_rule>();
}

} //namespace bv::pe
