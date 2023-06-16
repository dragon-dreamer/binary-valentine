#include "binary_valentine/pe/rule/load_config_flags_rule.h"

#include <string_view>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/load_config/load_config_directory.h"

namespace bv::pe
{

class load_config_flags_rule final
	: public core::rule_base<load_config_flags_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_load_config_flags_rule";
	static constexpr auto reports = output::get_rule_reports<
		load_config_flags_rule,
		pe_report::executable_process_heap,
		pe_report::global_flags_overridden>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code && !info.is_boot;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::load_config::load_config_directory_details& load_config) const
	{
		check_process_heap_flags(reporter, load_config);
		check_global_flags(reporter, load_config);
	}

private:
	template<typename Reporter>
	static void check_process_heap_flags(Reporter& reporter,
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		const bool executable_heap = std::visit([](const auto& dir) {
			using enum pe_bliss::load_config::process_heap_flags::value;
			return static_cast<bool>(dir.get_process_heap_flags()
				& heap_create_enable_execute);
		}, load_config.get_value());

		if (executable_heap)
			reporter.template log<pe_report::executable_process_heap>();
	}

	template<typename Reporter>
	static void check_global_flags(Reporter& reporter,
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		const bool has_global_flags = std::visit([](const auto& dir) {
			return dir.get_global_flags_set() != 0u
				|| dir.get_global_flags_clear() != 0;
		}, load_config.get_value());

		if (has_global_flags)
			reporter.template log<pe_report::global_flags_overridden>();
	}
};

void load_config_flags_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<load_config_flags_rule>(shared_values);
}

} //namespace bv::pe
