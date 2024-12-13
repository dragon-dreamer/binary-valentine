#include "binary_valentine/pe/combined_rule/combined_cross_signature_check_rule.h"

#include <string>
#include <string_view>
#include <unordered_map>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/core/combined_rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/case_conv.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"
#include "pe_bliss2/security/image_authenticode_verifier.h"

namespace bv::pe
{

class combined_cross_signature_check_rule final
	: public core::combined_rule_base<combined_cross_signature_check_rule>
{
public:
	using dependencies_type = core::combined_dependencies<
		core::filtered_value<core::value_tuple<
			const basic_pe_info&,
			// Note: this status structure references buffers from the
			// image security directory which will be deleted by the time
			// this rule is executed. It is fine to use the status
			// to check if the binary is signed (without accessing the
			// members which reference the security directory).
			// Once those members are also needed, it is necessary
			// to add a dependency to `pe_bliss::security::security_directory_details`
			// here in addition to `image_authenticode_check_status`.
			const pe_bliss::security::image_authenticode_check_status&,
			const pe_bliss::imports::import_directory_details*,
			const pe_bliss::delay_import::delay_import_directory_details*>
		>,
		core::unfiltered_value<core::value_tuple<
				const basic_pe_info&,
				const pe_bliss::security::image_authenticode_check_status*
			>,
			core::value_options::without_reporter
		>>;

	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_combined_cross_signature_check_rule";
	static constexpr auto reports = output::get_rule_reports<
		combined_cross_signature_check_rule,
		pe_report::signed_executable_imported_dll_is_unsigned,
		pe_report::signed_executable_delay_imported_dll_is_unsigned>();

	struct signature_status
	{
		bool is_signed{};
	};

	void run(auto&& imports_tuple, auto&& pe_infos_tuple) const
	{
		std::unordered_map<std::string, signature_status> path_with_lowercase_name_to_status;

		for (auto&& [info, authenticode_status] : pe_infos_tuple)
		{
			path_with_lowercase_name_to_status.try_emplace(
				info.canonical_parent_path + '/' + string::to_lower_ascii(info.file_name),
				signature_status{
					.is_signed = authenticode_status != nullptr && !authenticode_status->error
				});
		}

		for (auto&& [info, authenticode_status, imports, delay_imports, reporter] : imports_tuple)
		{
			check_import_directory<pe_report::signed_executable_imported_dll_is_unsigned>(
				authenticode_status, imports, info, path_with_lowercase_name_to_status, reporter);
			check_import_directory<pe_report::signed_executable_delay_imported_dll_is_unsigned>(
				authenticode_status, delay_imports, info, path_with_lowercase_name_to_status, reporter);
		}
	}

private:
	template<typename Report>
	void check_import_directory(
		const pe_bliss::security::image_authenticode_check_status& authenticode_status,
		const auto* imports,
		const basic_pe_info& info,
		const std::unordered_map<std::string, signature_status>& path_with_lowercase_name_to_status,
		auto& reporter) const
	{
		if (!imports || authenticode_status.error)
			return;

		std::visit([&reporter, &info, &path_with_lowercase_name_to_status](
				const auto& library_list) {
			for (const auto& lib : library_list)
			{
				const std::string& library_name = lib.get_library_name().value();
				auto it = path_with_lowercase_name_to_status.find(
					info.canonical_parent_path + '/' + string::to_lower_ascii(library_name));
				if (it == path_with_lowercase_name_to_status.cend())
					return;

				if (!it->second.is_signed)
				{
					reporter.template log<Report>(
						output::named_arg("imported_library", library_name));
				}
			}
		}, imports->get_list());
	}
};

void combined_cross_signature_check_rule_factory::add_rule(core::combined_rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<combined_cross_signature_check_rule>(shared_values);
}

} //namespace bv::pe
