#include "binary_valentine/pe/rule/export_rule.h"

#include <memory>
#include <string_view>
#include <utility>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/exports/export_directory.h"
#include "pe_bliss2/image/image.h"

namespace bv::pe
{

class export_rule final
	: public core::rule_base<export_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_export_rule";
	static constexpr auto reports = output::get_rule_reports<
		export_rule,
		pe_report::export_library_name_mismatch,
		pe_report::non_dll_exports_functions>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::exports::export_directory_details& dir,
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		const pe_bliss::image::image& image,
		const basic_pe_info& info) const
	{
		//Empty case is handled by export_directory_format_rule
		if (!dir.get_library_name().value().empty())
		{
			if (info.file_name != dir.get_library_name().value())
			{
				reporter.template log<pe_report::export_library_name_mismatch>(
					output::named_arg("file_name", info.file_name),
					output::named_arg("library_name", dir.get_library_name().value()));
			}
		}

		if (!image.get_file_header().is_dll())
			reporter.template log<pe_report::non_dll_exports_functions>();
	}
};

void export_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<export_rule>(shared_values);
}

} //namespace bv::pe
