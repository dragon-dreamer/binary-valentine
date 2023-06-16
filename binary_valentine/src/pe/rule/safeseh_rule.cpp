#include "binary_valentine/pe/rule/safeseh_rule.h"

#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/load_config/load_config_directory.h"

namespace bv::pe
{

class safeseh_rule final
	: public core::rule_base<safeseh_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_safeseh_rule";
	static constexpr auto reports = output::get_rule_reports<
		safeseh_rule,
		pe_report::no_safeseh>();

	bool is_applicable(const pe_bliss::image::image& image,
		const basic_pe_info& info) const
	{
		if (image.is_64bit())
			return false;

		if (image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::no_seh)
		{
			return false;
		}

		return !info.is_xbox && info.has_executable_code;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config) const
	{
		if (!load_config)
		{
			reporter.template log<pe_report::no_safeseh>();
			return;
		}

		const auto& dir = std::get<
			pe_bliss::load_config::load_config_directory_details::underlying_type32>(
			load_config->get_value());

		if (!dir.get_safeseh_handler_table().has_value())
			reporter.template log<pe_report::no_safeseh>();
	}
};

void safeseh_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<safeseh_rule>(shared_values);
}

} //namespace bv::pe
