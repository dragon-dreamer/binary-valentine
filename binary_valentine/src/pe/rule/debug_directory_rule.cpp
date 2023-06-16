#include "binary_valentine/pe/rule/debug_directory_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/debug/debug_directory.h"

namespace bv::pe
{

class debug_directory_rule final
	: public core::rule_base<debug_directory_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_debug_directory_rule";
	static constexpr auto reports = output::get_rule_reports<
		debug_directory_rule,
		pe_report::coff_mpdb_debug_directory_present,
		pe_report::debug_directory_present,
		pe_report::mpx_debug_directory_present,
		pe_report::pogo_optimization_pgi_status>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::debug::debug_directory_list_details& debug_dir) const
	{
		bool has_mpdb_coff = false, has_mpx = false, has_debug = false;
		for (const auto& entry : debug_dir.get_entries())
		{
			switch (entry.get_type())
			{
			case pe_bliss::debug::debug_directory_type::mpdb:
			case pe_bliss::debug::debug_directory_type::coff:
				has_mpdb_coff = true;
				break;

			case pe_bliss::debug::debug_directory_type::mpx:
				has_mpx = true;
				break;

			case pe_bliss::debug::debug_directory_type::pogo:
				check_pogo_status(reporter, entry);
				break;

			case pe_bliss::debug::debug_directory_type::codeview:
			case pe_bliss::debug::debug_directory_type::fpo:
			case pe_bliss::debug::debug_directory_type::borland:
			case pe_bliss::debug::debug_directory_type::pdbhash:
				has_debug = true;
				break;

			default:
				break;
			}
		}

		if (has_mpdb_coff)
			reporter.template log<pe_report::coff_mpdb_debug_directory_present>();
		if (has_mpx)
			reporter.template log<pe_report::mpx_debug_directory_present>();
		if (has_debug)
			reporter.template log<pe_report::debug_directory_present>();
	}
	
private:
	template<typename Reporter>
	static void check_pogo_status(Reporter& reporter,
		const pe_bliss::debug::debug_directory_details& debug_dir)
	{
		auto underlying_dir = debug_dir.get_underlying_directory({});
		const auto& pogo_dir = std::get<
			pe_bliss::debug::pogo_debug_directory_details>(underlying_dir);
		if (pogo_dir.get_pogo_type() == pe_bliss::debug::pogo_type::pgi)
			reporter.template log<pe_report::pogo_optimization_pgi_status>();
	}
};

void debug_directory_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<debug_directory_rule>(shared_values);
}

} //namespace bv::pe
