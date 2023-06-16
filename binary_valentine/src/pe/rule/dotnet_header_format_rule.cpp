#include "binary_valentine/pe/rule/dotnet_header_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/dotnet/dotnet_directory.h"

namespace bv::pe
{

class dotnet_header_format_rule final
	: public core::rule_base<dotnet_header_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_dotnet_header_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		dotnet_header_format_rule,
		pe_report::dotnet_header_format_error>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::dotnet::cor20_header_details& header) const
	{
		error_helpers::report_errors<pe_report::dotnet_header_format_error>(
			reporter, header, error_code_to_message_id_);
	}

private:
	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::dotnet::dotnet_directory_errc;
		return {
			{ unable_to_deserialize_header, "pe_dotnet_unable_to_deserialize_header" },
			{ unable_to_deserialize_footer, "pe_dotnet_unable_to_deserialize_footer" },
			{ version_length_not_aligned, "pe_dotnet_version_length_not_aligned" },
			{ unable_to_deserialize_runtime_version, "pe_unable_to_deserialize_runtime_version" },
			{ too_long_runtime_version_string, "pe_dotnet_too_long_runtime_version_string" },
			{ unable_to_deserialize_stream_header, "pe_dotnet_unable_to_deserialize_stream_header" },
			{ unable_to_deserialize_stream_data, "pe_dotnet_unable_to_deserialize_stream_data" },
			{ duplicate_stream_names, "pe_dotnet_duplicate_stream_names" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void dotnet_header_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<dotnet_header_format_rule>(shared_values);
}

} //namespace bv::pe
