#include "binary_valentine/pe/rule/import_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"
#include "pe_bliss2/imports/import_directory_loader.h"

namespace bv::pe
{

class import_format_rule final
	: public core::rule_base<import_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_import_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		import_format_rule,
		pe_report::import_directory_format_error,
		pe_report::delay_import_directory_format_error>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::imports::import_directory_details* imports,
		const pe_bliss::delay_import::delay_import_directory_details* delay_imports) const
	{
		check_format_errors<pe_report::import_directory_format_error>(reporter, imports);
		check_format_errors<pe_report::delay_import_directory_format_error>(
			reporter, delay_imports);
	}

private:
	template<typename Report, typename Reporter, typename Directory>
	static void check_format_errors(Reporter& reporter, const Directory* dir)
	{
		if (!dir)
			return;

		std::visit([&reporter] (const auto& libraries) {
			check_format_errors<Report>(reporter, libraries);
		}, dir->get_list());
	}

	template<typename Report, typename Reporter, typename Libraries>
	static void check_format_errors(Reporter& reporter, const Libraries& libraries)
	{
		error_helpers::report_errors<Report>(
			reporter, libraries, error_code_to_message_id_);
	}

	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::imports::import_directory_loader_errc;
		return {
			{ invalid_library_name, "pe_import_invalid_library_name" },
			{ invalid_import_hint, "pe_import_invalid_import_hint" },
			{ invalid_import_name, "pe_import_invalid_import_name" },
			{ invalid_hint_name_rva, "pe_import_invalid_hint_name_rva" },
			{ lookup_and_address_table_thunks_differ,
				"pe_import_lookup_and_address_table_thunks_differ" },
			{ invalid_import_directory, "pe_import_invalid_import_directory" },
			{ zero_iat_and_ilt, "pe_import_zero_iat_and_ilt" },
			{ invalid_import_ordinal, "pe_import_invalid_import_ordinal" },
			{ zero_iat, "pe_import_zero_iat" },
			{ invalid_imported_library_iat_ilt,
				"pe_import_invalid_imported_library_iat_ilt" },
			{ empty_library_name, "pe_import_empty_library_name" },
			{ empty_import_name, "pe_import_empty_import_name" },
			{ address_and_unload_table_thunks_differ,
				"pe_import_address_and_unload_table_thunks_differ" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void import_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<import_format_rule>(shared_values);
}

} //namespace bv::pe
