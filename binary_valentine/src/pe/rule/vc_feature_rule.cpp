#include "binary_valentine/pe/rule/vc_feature_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/import_based_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/debug/debug_directory.h"

namespace bv::pe
{

class vc_feature_rule final
	: public core::rule_base<vc_feature_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_vc_feature_rule";
	static constexpr auto reports = output::get_rule_reports<
		vc_feature_rule,
		pe_report::sdl_not_always_used,
		pe_report::pre_cpp11_used>();

	bool is_applicable(const basic_pe_info& info,
		const import_based_info& import_based) const
	{
		return info.has_executable_code
			&& !info.is_il_only && !info.is_pre_win7ce && !info.is_boot
			&& !import_based.is_native_universal_windows_platform;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::debug::vc_feature_debug_directory_details& vc_feature) const
	{
		const auto& descriptor = vc_feature.get_descriptor().get();

		if (descriptor.pre_vc_plus_plus_11_count)
		{
			reporter.template log<pe_report::pre_cpp11_used>(
				output::named_arg("count", descriptor.pre_vc_plus_plus_11_count));
		}
		
		if (!descriptor.sdl_count
			|| (descriptor.sdl_count < descriptor.c_and_c_plus_plus_count
				&& descriptor.c_and_c_plus_plus_count - descriptor.sdl_count > sdl_threshold))
		{
			reporter.template log<pe_report::sdl_not_always_used>(
				output::named_arg("count", descriptor.c_and_c_plus_plus_count - descriptor.sdl_count));
		}
	}

private:
	static constexpr std::uint32_t sdl_threshold = 150u;
};

void vc_feature_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<vc_feature_rule>(shared_values);
}

} //namespace bv::pe
