#include "binary_valentine/pe/rule/ansi_import_rule.h"

#include <cctype>
#include <string_view>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/shared_data/winapi_library_list.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"

namespace bv::pe
{

class ansi_import_rule final
	: public core::rule_base<ansi_import_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_ansi_import_rule";
	static constexpr auto reports = output::get_rule_reports<
		ansi_import_rule,
		pe_report::ansi_import,
		pe_report::ansi_delay_import>();

public:
	bool is_applicable(const basic_pe_info& info) const
	{
		return !info.is_boot;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const full_winapi_library_map_type& all_winapi,
		const pe_bliss::imports::import_directory_details* imports,
		const pe_bliss::delay_import::delay_import_directory_details* delay_imports) const
	{
		if (imports)
			check_imports(reporter, all_winapi, *imports, false);
		if (delay_imports)
			check_imports(reporter, all_winapi, *delay_imports, true);
	}

private:
	template<typename Reporter, typename Directory>
	void check_imports(Reporter& reporter,
		const full_winapi_library_map_type& all_winapi,
		const Directory& dir, bool is_delayload) const
	{
		std::visit([&reporter, &all_winapi, is_delayload, this](const auto& libraries) {
			for (const auto& library : libraries)
				this->check_library(reporter, all_winapi, library, is_delayload);
		}, dir.get_list());
	}

	template<typename Reporter, typename Library>
	void check_library(Reporter& reporter,
		const full_winapi_library_map_type& all_winapi,
		const Library& library, bool is_delayload) const
	{
		if (!all_winapi.get_library(library.get_library_name().value()))
			return;

		for (const auto& import : library.get_imports())
		{
			check_api(reporter, import,
				library.get_library_name().value(), is_delayload);
		}
	}

	template<typename Reporter, typename Import>
	void check_api(Reporter& reporter, const Import& import,
		const std::string& library_name, bool is_delayload) const
	{
		const auto* hint_name = std::get_if<typename Import::hint_name_type>(
				&import.get_import_info());
		if (!hint_name)
			return;

		const auto& name = hint_name->get_name().value();
		if (name.size() < 2u || !name.ends_with('A'))
			return;

		char prev_letter = name[name.size() - 2];
		if (!std::islower(static_cast<unsigned char>(prev_letter)))
			return;

		std::string unicode_api_name = name;
		unicode_api_name.back() = 'W';

		if (is_delayload)
		{
			reporter.template log<pe_report::ansi_delay_import>(
				output::named_arg("dll", library_name),
				output::named_arg("api", name),
				output::named_arg("unicode_api", std::move(unicode_api_name)));
		}
		else
		{
			reporter.template log<pe_report::ansi_import>(
				output::named_arg("dll", library_name),
				output::named_arg("api", name),
				output::named_arg("unicode_api", std::move(unicode_api_name)));
		}
	}
};

void ansi_import_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<ansi_import_rule>();
}

} //namespace bv::pe
