#include "binary_valentine/pe/rule/rich_data_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/rich/compid_database.h"
#include "pe_bliss2/rich/rich_header.h"

namespace bv::pe
{

class rich_data_rule final
	: public core::rule_base<rich_data_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_rich_data_rule";
	static constexpr auto reports = output::get_rule_reports<
		rich_data_rule,
		pe_report::tool_potential_preview_version,
		pe_report::tool_preview_version,
		pe_report::too_old_product_version>();

	static constexpr auto too_old_version
		= pe_bliss::rich::compid_database::product_type::visual_studio_2017_15_9_30;

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::rich::rich_header& rich_header) const
	{
		const auto& compids = rich_header.get_compids();
		for (const auto& compid : compids)
		{
			const auto tool = pe_bliss::rich::compid_database::get_tool(compid.prod_id);
			auto product_info = pe_bliss::rich::compid_database::get_product(compid);
			auto product_type_string = pe_bliss::rich::compid_database::product_type_to_string(
				product_info.type);
			const auto tool_type = pe_bliss::rich::compid_database::tool_type_to_string(tool);
			if (product_type_string.find("Preview") != std::string_view::npos)
			{
				if (product_info.exact)
				{
					reporter.template log<pe_report::tool_preview_version>(
						output::named_arg("version", product_type_string),
						output::named_arg("type", tool_type));
				}
				else
				{
					reporter.template log<pe_report::tool_potential_preview_version>(
						output::named_arg("version", product_type_string),
						output::named_arg("type", tool_type));
				}
			}

			if (tool == pe_bliss::rich::compid_database::tool_type::imported_symbol)
				continue;

			if (static_cast<std::uint32_t>(product_info.type)
				<= static_cast<std::uint32_t>(too_old_version))
			{
				reporter.template log<pe_report::too_old_product_version>(
					output::named_arg("version", product_type_string),
					output::named_arg("type", tool_type));
			}
		}
	}
};

void rich_data_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<rich_data_rule>();
}

} //namespace bv::pe
