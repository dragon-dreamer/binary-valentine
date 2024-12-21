#include "binary_valentine/pe/rule/debug_directory_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/debug/debug_directory.h"
#include "pe_bliss2/debug/debug_directory_loader.h"

namespace bv::pe
{

class debug_directory_format_rule final
	: public core::rule_base<debug_directory_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_debug_directory_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		debug_directory_format_rule,
		pe_report::debug_directory_format_error>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::debug::debug_directory_list_details& header) const
	{
		error_helpers::report_errors<pe_report::debug_directory_format_error>(
			reporter, header, error_code_to_message_id_);
		error_helpers::report_errors<pe_report::debug_directory_format_error>(
			reporter, header.get_entries(), error_code_to_message_id_);
	}

private:
	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::debug::debug_directory_loader_errc;
		return {
			{ no_rva_and_file_offset, "pe_debug_no_rva_and_file_offset" },
			{ invalid_file_offset, "pe_debug_invalid_file_offset" },
			{ invalid_debug_directory_size, "pe_debug_invalid_debug_directory_size" },
			{ excessive_data_in_directory, "pe_debug_excessive_data_in_directory" },
			{ unable_to_load_entries, "pe_debug_unable_to_load_entries" },
			{ unable_to_load_raw_data, "pe_debug_unable_to_load_raw_data" },
			{ rva_and_file_offset_do_not_match, "pe_debug_rva_and_file_offset_do_not_match" },
			{ too_many_debug_directories, "pe_debug_too_many_debug_directories" },
			{ too_big_raw_data, "pe_debug_too_big_raw_data" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void debug_directory_format_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<debug_directory_format_rule>();
}

} //namespace bv::pe
