#include "binary_valentine/pe/rule/export_directory_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/exports/export_directory.h"
#include "pe_bliss2/exports/export_directory_loader.h"

namespace bv::pe
{

class export_directory_format_rule final
	: public core::rule_base<export_directory_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_export_directory_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		export_directory_format_rule,
		pe_report::export_directory_format_error>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::exports::export_directory_details& header) const
	{
		error_helpers::report_errors<pe_report::export_directory_format_error>(
			reporter, header, error_code_to_message_id_);
		error_helpers::report_errors<pe_report::export_directory_format_error>(
			reporter, header.get_export_list(), error_code_to_message_id_);

		pe_bliss::error_list merged;
		for (const auto& address : header.get_export_list())
			error_helpers::merge_errors(address.get_names(), merged);

		error_helpers::report_errors<pe_report::export_directory_format_error>(
			reporter, merged, error_code_to_message_id_);
	}

private:
	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::exports::export_directory_loader_errc;
		return {
			{ invalid_library_name, "pe_exports_invalid_library_name" },
			{ invalid_forwarded_name, "pe_exports_invalid_forwarded_name" },
			{ invalid_name_list, "pe_exports_invalid_name_list" },
			{ invalid_address_list, "pe_exports_invalid_address_list" },
			{ invalid_name_ordinal, "pe_exports_invalid_name_ordinal" },
			{ invalid_name_rva, "pe_exports_invalid_name_rva" },
			{ empty_name, "pe_exports_empty_name" },
			{ unsorted_names, "pe_exports_unsorted_names" },
			{ invalid_rva, "pe_exports_invalid_rva" },
			{ invalid_directory, "pe_exports_invalid_directory" },
			{ invalid_address_list_number_of_functions,
				"pe_exports_invalid_address_list_number_of_functions" },
			{ invalid_address_list_number_of_names,
				"pe_exports_invalid_address_list_number_of_names" },
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void export_directory_format_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<export_directory_format_rule>();
}

} //namespace bv::pe
