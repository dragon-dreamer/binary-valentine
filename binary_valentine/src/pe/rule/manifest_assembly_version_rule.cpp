#include "binary_valentine/pe/rule/manifest_assembly_version_rule.h"

#include <array>
#include <cstddef>
#include <format>
#include <string_view>
#include <utility>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/manifest_info.h"
#include "binary_valentine/pe/data/version_info_list.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

namespace bv::pe
{

class manifest_assembly_version_rule final
	: public core::rule_base<manifest_assembly_version_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_manifest_assembly_version_rule";
	static constexpr auto reports = output::get_rule_reports<
		manifest_assembly_version_rule,
		pe_report::manifest_assembly_version_version_info_version_mismatch>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const manifest_info& manifest,
		const version_info_list& version) const
	{
		if (!manifest.manifest)
			return;

		const auto& assembly_version = manifest.manifest
			->get_assembly_identity().get_version_raw();
		if (assembly_version.empty())
			return;

		const auto* info = find_first_valid_block(version);
		if (!info)
			return;

		auto file_version = info->get_file_version_info().get_file_version_string();
		auto product_version = info->get_file_version_info().get_product_version_string();
		if (assembly_version != file_version && assembly_version != product_version)
		{
			reporter.template log<
				pe_report::manifest_assembly_version_version_info_version_mismatch>(
					output::named_arg("assembly_version", assembly_version),
					output::named_arg("file_version", std::move(file_version)),
					output::named_arg("product_version", std::move(product_version)));
		}
	}
};

void manifest_assembly_version_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<manifest_assembly_version_rule>(shared_values);
}

} //namespace bv::pe
