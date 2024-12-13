#include "binary_valentine/pe/combined_rule/combined_dll_import_names_case_rule.h"

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
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"

namespace bv::pe
{

class combined_dll_import_names_case_rule final
	: public core::combined_rule_base<combined_dll_import_names_case_rule>
{
public:
	using dependencies_type = core::combined_dependencies<
		core::filtered_value<core::value_tuple<
			const basic_pe_info&,
			const pe_bliss::imports::import_directory_details*,
			const pe_bliss::delay_import::delay_import_directory_details*>
		>,
		core::unfiltered_value<const basic_pe_info&, core::value_options::without_reporter>>;

	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_combined_dll_import_names_case_rule";
	static constexpr auto reports = output::get_rule_reports<
		combined_dll_import_names_case_rule,
		pe_report::imported_dll_name_case_mismatch,
		pe_report::delay_imported_dll_name_case_mismatch>();

	void run(auto&& imports_tuple, auto&& pe_infos) const
	{
		std::unordered_map<std::string, std::string_view> lowercase_name_to_original_name;
		for (const auto& info : pe_infos)
		{
			lowercase_name_to_original_name.try_emplace(
				info.canonical_parent_path + '/' + string::to_lower_ascii(info.file_name),
				info.file_name);
		}

		for (auto&& [info, imports, delay_imports, reporter] : imports_tuple)
		{
			check_import_directory<pe_report::imported_dll_name_case_mismatch>(
				imports, info, lowercase_name_to_original_name, reporter);
			check_import_directory<pe_report::delay_imported_dll_name_case_mismatch>(
				delay_imports, info, lowercase_name_to_original_name, reporter);
		}
	}

private:
	template<typename Report>
	void check_import_directory(
		const auto* imports,
		const basic_pe_info& info,
		const std::unordered_map<std::string, std::string_view>& lowercase_name_to_original_name,
		auto& reporter) const
	{
		if (!imports)
			return;

		std::visit([&reporter, &info, &lowercase_name_to_original_name](const auto& library_list) {
			for (const auto& lib : library_list)
			{
				const std::string& library_name = lib.get_library_name().value();
				auto it = lowercase_name_to_original_name.find(
					info.canonical_parent_path + '/' + string::to_lower_ascii(library_name));
				if (it != lowercase_name_to_original_name.cend())
				{
					if (it->second != library_name)
					{
						reporter.template log<Report>(
							output::named_arg("imported_library", library_name),
							output::named_arg("library_file_name", std::string(it->second)));
					}
				}
			}
		}, imports->get_list());
	}
};

void combined_dll_import_names_case_rule_factory::add_rule(core::combined_rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<combined_dll_import_names_case_rule>(shared_values);
}

} //namespace bv::pe
