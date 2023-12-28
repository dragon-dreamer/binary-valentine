#include "binary_valentine/pe/rule/security_directory_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/security/security_directory.h"
#include "pe_bliss2/security/security_directory_loader.h"

namespace bv::pe
{

class security_directory_format_rule final
	: public core::rule_base<security_directory_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_security_directory_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		security_directory_format_rule,
		pe_report::security_directory_format_error>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::security::security_directory_details& header) const
	{
		error_helpers::report_errors<pe_report::security_directory_format_error>(
			reporter, header, error_code_to_message_id_);
		error_helpers::report_errors<pe_report::security_directory_format_error>(
			reporter, header.get_entries(), error_code_to_message_id_);
	}

private:
	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::security::security_directory_loader_errc;
		return {
			{ invalid_directory, "pe_security_invalid_directory" },
			{ invalid_entry, "pe_security_invalid_entry" },
			{ invalid_certificate_data, "pe_security_invalid_certificate_data" },
			{ invalid_entry_size, "pe_security_invalid_entry_size" },
			{ invalid_directory_size, "pe_security_invalid_directory_size" },
			{ unaligned_directory, "pe_security_unaligned_directory" },
			{ too_many_entries, "pe_security_too_many_entries" },
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void security_directory_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<security_directory_format_rule>(shared_values);
}

} //namespace bv::pe
