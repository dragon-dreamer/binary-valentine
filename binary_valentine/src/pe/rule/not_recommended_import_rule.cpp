#include "binary_valentine/pe/rule/not_recommended_import_rule.h"

#include <string_view>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/shared_data/not_recommended_imports.h"
#include "binary_valentine/string/case_conv.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/imports/import_directory.h"

namespace bv::pe
{

class not_recommended_import_rule final
	: public core::rule_base<not_recommended_import_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_not_recommended_import_rule";
	static constexpr auto reports = output::get_rule_reports<
		not_recommended_import_rule,
		pe_report::dangerous_winapi_import,
		pe_report::deprecated_winapi_import,
		pe_report::insecure_winapi_import,
		pe_report::internal_winapi_import,
		pe_report::dangerous_winapi_delay_import,
		pe_report::deprecated_winapi_delay_import,
		pe_report::insecure_winapi_delay_import,
		pe_report::internal_winapi_delay_import,
		pe_report::xbox_only_winapi_import,
		pe_report::xbox_only_winapi_delay_import>();

	using constructor_dependencies_type = core::dependencies<const not_recommended_imports&>;

public:
	explicit not_recommended_import_rule(const not_recommended_imports& imports) noexcept
		: not_recommended_imports_(imports)
	{
	}

	bool is_applicable(const basic_pe_info& info) const
	{
		return !info.is_boot;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::imports::import_directory_details* imports,
		const pe_bliss::delay_import::delay_import_directory_details* delay_imports,
		const basic_pe_info& info) const
	{
		if (imports)
			check_imports(reporter, *imports, info.is_xbox, false);
		if (delay_imports)
			check_imports(reporter, *delay_imports, info.is_xbox, true);
	}

private:
	template<typename Reporter, typename Directory>
	void check_imports(Reporter& reporter, const Directory& dir,
		bool is_xbox, bool is_delayload) const
	{
		std::visit([&reporter, is_delayload, is_xbox, this](const auto& libraries) {
			for (const auto& library : libraries)
				this->check_library(reporter, library, is_xbox, is_delayload);
		}, dir.get_list());
	}

	template<typename Reporter, typename Library>
	void check_library(Reporter& reporter, const Library& library,
		bool is_xbox, bool is_delayload) const
	{
		const auto* dll = not_recommended_imports_.get_library(
			library.get_library_name().value());
		if (!dll)
			return;

		for (const auto& import : library.get_imports())
		{
			check_api(reporter, import, *dll,
				library.get_library_name().value(), is_xbox, is_delayload);
		}
	}

	template<typename Reporter, typename Import>
	void check_api(Reporter& reporter, const Import& import,
		const not_recommended_api_map& dll,
		const std::string& library_name, bool is_xbox, bool is_delayload) const
	{
		const auto* hint_name = std::get_if<typename Import::hint_name_type>(
				&import.get_import_info());
		if (!hint_name)
			return;

		const auto* api = dll.get_api(hint_name->get_name().value());
		if (!api)
			return;

		if (is_delayload)
		{
			if (api->get_flags() & api_flags::dangerous)
			{
				report_with_api<pe_report::dangerous_winapi_delay_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::deprecated)
			{
				report_with_api<pe_report::deprecated_winapi_delay_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::insecure)
			{
				report_with_api<pe_report::insecure_winapi_delay_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::internal)
			{
				report_with_api<pe_report::internal_winapi_delay_import>(
					reporter, api, library_name, hint_name);
			}
		}
		else
		{
			if (api->get_flags() & api_flags::dangerous)
			{
				report_with_api<pe_report::dangerous_winapi_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::deprecated)
			{
				report_with_api<pe_report::deprecated_winapi_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::insecure)
			{
				report_with_api<pe_report::insecure_winapi_import>(
					reporter, api, library_name, hint_name);
			}
			if (api->get_flags() & api_flags::internal)
			{
				report_with_api<pe_report::internal_winapi_import>(
					reporter, api, library_name, hint_name);
			}
		}

		if (!is_xbox && (api->get_flags() & api_flags::xbox_only))
		{
			if (is_delayload)
			{
				report_with_api<pe_report::xbox_only_winapi_delay_import>(
					reporter, api, library_name, hint_name);
			}
			else
			{
				report_with_api<pe_report::xbox_only_winapi_import>(
					reporter, api, library_name, hint_name);
			}
		}
	}

	template<typename Report, typename Reporter, typename HintName>
	static void report_with_api(Reporter& reporter,
		const not_recommended_api* api,
		const std::string& library_name, const HintName* hint_name)
	{
		reporter.template log<Report>(
			output::named_arg("dll", library_name),
			output::named_arg("api", hint_name->get_name().value()),
			output::named_arg("mitigation", api->get_mitigation()));
	}

private:
	const not_recommended_imports& not_recommended_imports_;
};

void not_recommended_import_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<not_recommended_import_rule>(shared_values);
}

} //namespace bv::pe
