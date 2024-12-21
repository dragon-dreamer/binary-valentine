#include "binary_valentine/pe/rule/cet_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/debug/debug_directory.h"

namespace bv::pe
{

class cet_rule final
	: public core::rule_base<cet_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_cet_rule";
	static constexpr auto reports = output::get_rule_reports<
		cet_rule,
		pe_report::cet_not_enabled>();

	bool is_applicable(const basic_pe_info& info) const
	{
		using enum pe_bliss::core::file_header::machine_type;
		return info.has_executable_code
			&& !info.is_il_only && !info.is_pre_win7ce && !info.is_boot
			&& (info.machine_type == i386 || info.machine_type == i486
				|| info.machine_type == pentium || info.machine_type == ia64
				|| info.machine_type == amd64 || info.machine_type == tahoe);
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::debug::debug_directory_list_details* debug_dir) const
	{
		if (!has_cet(find_ex_dll_characteristics_debug_dir(debug_dir)))
			reporter.template log<pe_report::cet_not_enabled>();
	}

private:
	static const pe_bliss::debug::debug_directory_details*
		find_ex_dll_characteristics_debug_dir(
			const pe_bliss::debug::debug_directory_list_details* debug_dir)
	{
		if (!debug_dir)
			return nullptr;

		for (const auto& entry : debug_dir->get_entries())
		{
			if (entry.get_type()
				== pe_bliss::debug::debug_directory_type::ex_dllcharacteristics)
			{
				return &entry;
			}
		}

		return nullptr;
	}

	static bool has_cet(const pe_bliss::debug::debug_directory_details* dll_characteristics_dir)
	{
		if (!dll_characteristics_dir)
			return false;

		const auto dll_characteristics
			= std::get<pe_bliss::debug::ex_dllcharacteristics_debug_directory_details>(
				dll_characteristics_dir->get_underlying_directory());
		if (dll_characteristics.has_errors())
			return false;

		return static_cast<bool>(dll_characteristics.get_characteristics()
			& pe_bliss::debug::image_dllcharacteristics_ex::cet_compat);
	}
};

void cet_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<cet_rule>();
}

} //namespace bv::pe
