#include "binary_valentine/pe/rule/manifest_format_rule.h"

#include <exception>
#include <string_view>
#include <system_error>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/manifest_info.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/resources/manifest.h"
#include "pe_bliss2/resources/manifest_accessor_interface.h"

namespace bv::pe
{

class manifest_format_rule final
	: public core::rule_base<manifest_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_manifest_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		manifest_format_rule,
		pe_report::manifest_load_error,
		pe_report::manifest_format_error,
		pe_report::app_manifest_unsupported_element,
		pe_report::assembly_manifest_unsupported_element>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code
			&& !info.is_pre_win7ce && !info.is_boot
			&& !info.is_driver;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const manifest_info& manifest,
		const pe_bliss::image::image& image) const
	{
		if (manifest.type == manifest_type::no_manifest)
			return;

		if (image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::no_isolation)
		{
			return;
		}

		if (!manifest.manifest)
		{
			try
			{
				std::rethrow_exception(manifest.manifest_load_error);
			}
			catch (const std::exception&)
			{
				reporter.template log<pe_report::manifest_load_error>(
					output::current_exception_arg());
			}
			return;
		}

		check_format_errors(reporter, manifest.manifest.value());

		if (manifest.type == manifest_type::application)
			check_application_manifest_elements(reporter, manifest.manifest.value());
		else
			check_assembly_manifest_elements(reporter, manifest.manifest.value());
	}

private:
	template<typename Reporter>
	static void report_app_unsupported_tag(Reporter& reporter, std::string_view tag)
	{
		reporter.template log<pe_report::app_manifest_unsupported_element>(
			output::named_arg("tag", tag));
	}

	template<typename Reporter>
	static void report_assembly_unsupported_tag(Reporter& reporter, std::string_view tag)
	{
		reporter.template log<pe_report::assembly_manifest_unsupported_element>(
			output::named_arg("tag", tag));
	}

	template<typename Reporter>
	static void check_assembly_manifest_elements(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (manifest.no_inherit()
			== pe_bliss::resources::assembly_no_inherit::no_inherit)
		{
			report_assembly_unsupported_tag(reporter, "<noInherit>");
		}

		if (manifest.get_supported_os_list())
			report_assembly_unsupported_tag(reporter, "<compatibility>");

		if (manifest.get_requested_privileges())
			report_assembly_unsupported_tag(reporter, "<trustInfo>");

		if (manifest.get_active_code_page())
			report_assembly_unsupported_tag(reporter, "<activeCodePage>");

		if (manifest.auto_elevate())
			report_assembly_unsupported_tag(reporter, "<autoElevate>");

		if (manifest.disable_theming())
			report_assembly_unsupported_tag(reporter, "<disableTheming>");

		if (manifest.disable_window_filtering())
			report_assembly_unsupported_tag(reporter, "<disableWindowFiltering>");

		if (manifest.get_dpi_awareness().get_dpi_aware_raw())
			report_assembly_unsupported_tag(reporter, "<dpiAware>");
		if (manifest.get_dpi_awareness().get_dpi_awareness_raw())
			report_assembly_unsupported_tag(reporter, "<dpiAwareness>");

		if (manifest.gdi_scaling())
			report_assembly_unsupported_tag(reporter, "<gdiScaling>");

		if (manifest.high_resolution_scrolling_aware())
			report_assembly_unsupported_tag(reporter, "<highResolutionScrollingAware>");

		if (manifest.long_path_aware())
			report_assembly_unsupported_tag(reporter, "<longPathAware>");

		if (manifest.printer_driver_isolation())
			report_assembly_unsupported_tag(reporter, "<printerDriverIsolation>");

		if (manifest.ultra_high_resolution_scrolling_aware())
			report_assembly_unsupported_tag(reporter, "<ultraHighResolutionScrollingAware>");

		if (manifest.get_msix_identity())
			report_assembly_unsupported_tag(reporter, "<msix>");

		if (manifest.get_heap_type())
			report_assembly_unsupported_tag(reporter, "<heapType>");
	}

	template<typename Reporter>
	static void check_application_manifest_elements(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (manifest.no_inherit()
			== pe_bliss::resources::assembly_no_inherit::no_inheritable)
		{
			report_app_unsupported_tag(reporter, "<noInheritable>");
		}

		bool has_com_classes = false;
		bool has_com_typelibs = false;
		bool has_window_classes = false;
		bool has_com_interface_proxy_stubs = false;
		for (const auto& file : manifest.get_files())
		{
			if (!file.get_com_classes().empty())
				has_com_classes = true;
			if (!file.get_com_typelibs().empty())
				has_com_typelibs = true;
			if (!file.get_window_classes().empty())
				has_window_classes = true;
			if (!file.get_com_interface_proxy_stubs().empty())
				has_com_interface_proxy_stubs = true;
		}

		if (has_com_classes)
			report_app_unsupported_tag(reporter, "<comClass>");
		if (has_com_typelibs)
			report_app_unsupported_tag(reporter, "<typelib>");
		if (has_window_classes)
			report_app_unsupported_tag(reporter, "<windowClass>");
		if (has_com_interface_proxy_stubs)
			report_app_unsupported_tag(reporter, "<comInterfaceProxyStub>");

		if (!manifest.get_com_interface_external_proxy_stubs().empty())
			report_app_unsupported_tag(reporter, "<comInterfaceExternalProxyStub>");
	}

	template<typename Reporter>
	static void check_format_errors(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		error_helpers::report_errors<pe_report::manifest_format_error>(
			reporter, manifest, error_code_to_message_id_);
		error_helpers::report_errors<pe_report::manifest_format_error>(
			reporter, manifest.get_assembly_identity(),
			error_code_to_message_id_);

		if (manifest.get_supported_os_list())
		{
			error_helpers::report_errors<pe_report::manifest_format_error>(
				reporter, *manifest.get_supported_os_list(),
				error_code_to_message_id_);
		}

		if (manifest.get_msix_identity())
		{
			error_helpers::report_errors<pe_report::manifest_format_error>(
				reporter, *manifest.get_msix_identity(),
				error_code_to_message_id_);
		}

		error_helpers::report_errors<pe_report::manifest_format_error>(
			reporter, manifest.get_dependencies(), error_code_to_message_id_);
		error_helpers::report_errors<pe_report::manifest_format_error>(
			reporter, manifest.get_files(), error_code_to_message_id_);
		error_helpers::report_errors<pe_report::manifest_format_error>(
			reporter, manifest.get_com_interface_external_proxy_stubs(),
			error_code_to_message_id_);
	}

	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::resources::manifest_errc;
		return {
			{ invalid_manifest_version, "pe_manifest_invalid_manifest_version" },
			{ empty_manifest, "pe_manifest_empty_manifest" },
			{ absent_manifest_version, "pe_manifest_absent_manifest_version" },
			{ no_inherit_element_not_first, "pe_manifest_no_inherit_element_not_first" },
			{ absent_assembly_identity, "pe_manifest_absent_assembly_identity" },
			{ invalid_assembly_identity_element_position,
				"pe_manifest_invalid_assembly_identity_element_position" },
			{ multiple_assembly_identities, "pe_manifest_multiple_assembly_identities" },
			{ multiple_no_inherit_elements, "pe_manifest_multiple_no_inherit_elements" },
			{ absent_assembly_identity_type, "pe_manifest_absent_assembly_identity_type" },
			{ absent_assembly_identity_name, "pe_manifest_absent_assembly_identity_name" },
			{ absent_assembly_identity_version,
				"pe_manifest_absent_assembly_identity_version" },
			{ multiple_compatibility_elements,
				"pe_manifest_multiple_compatibility_elements" },
			{ multiple_maxversiontested_elements,
				"pe_manifest_multiple_maxversiontested_elements" },
			{ invalid_dependencies, "pe_manifest_invalid_dependencies" },
			{ invalid_dependend_assembly_element_position,
				"pe_manifest_invalid_dependend_assembly_element_position" },
			{ absent_file_name, "pe_manifest_absent_file_name" },
			{ absent_clsid, "pe_manifest_absent_clsid" },
			{ absent_tlbid, "pe_manifest_absent_tlbid" },
			{ absent_version, "pe_manifest_absent_version" },
			{ absent_helpdir, "pe_manifest_absent_helpdir" },
			{ absent_iid, "pe_manifest_absent_iid" },
			{ multiple_application_elements, "pe_manifest_multiple_application_elements" },
			{ multiple_windows_settings_elements,
				"pe_manifest_multiple_windows_settings_elements" },
			{ multiple_active_code_page_elements,
				"pe_manifest_multiple_active_code_page_elements" },
			{ multiple_auto_elevate_elem, "pe_manifest_multiple_auto_elevate_elem" },
			{ invalid_auto_elevate_elem, "pe_manifest_invalid_auto_elevate_elem" },
			{ multiple_disable_theming_elem, "pe_manifest_multiple_disable_theming_elem" },
			{ invalid_disable_theming_elem, "pe_manifest_invalid_disable_theming_elem" },
			{ multiple_disable_window_filtering_elem,
				"pe_manifest_multiple_disable_window_filtering_elem" },
			{ invalid_disable_window_filtering_elem,
				"pe_manifest_invalid_disable_window_filtering_elem" },
			{ multiple_gdi_scaling_elem, "pe_manifest_multiple_gdi_scaling_elem" },
			{ invalid_gdi_scaling_elem, "pe_manifest_invalid_gdi_scaling_elem" },
			{ multiple_high_resolution_scrolling_aware_elem,
				"pe_manifest_multiple_high_resolution_scrolling_aware_elem" },
			{ invalid_high_resolution_scrolling_aware_elem,
				"pe_manifest_invalid_high_resolution_scrolling_aware_elem" },
			{ multiple_ultra_high_resolution_scrolling_aware_elem,
				"pe_manifest_multiple_ultra_high_resolution_scrolling_aware_elem" },
			{ invalid_ultra_high_resolution_scrolling_aware_elem,
				"pe_manifest_invalid_ultra_high_resolution_scrolling_aware_elem" },
			{ multiple_long_path_aware_elem, "pe_manifest_multiple_long_path_aware_elem" },
			{ invalid_long_path_aware_elem, "pe_manifest_invalid_long_path_aware_elem" },
			{ multiple_printer_driver_isolation_elem,
				"pe_manifest_multiple_printer_driver_isolation_elem" },
			{ invalid_printer_driver_isolation_elem,
				"pe_manifest_invalid_printer_driver_isolation_elem" },
			{ multiple_dpi_aware_elements, "pe_manifest_multiple_dpi_aware_elements" },
			{ multiple_dpi_awareness_elements, "pe_manifest_multiple_dpi_awareness_elements" },
			{ multiple_heap_type_elements, "pe_manifest_multiple_heap_type_elements" },
			{ multiple_trust_info_elements, "pe_manifest_multiple_trust_info_elements" },
			{ multiple_security_elements, "pe_manifest_multiple_security_elements" },
			{ multiple_requested_privileges_elements,
				"pe_manifest_multiple_requested_privileges_elements" },
			{ multiple_requested_execution_level_elements,
				"pe_manifest_multiple_requested_execution_level_elements" },
			{ absent_requested_execution_level, "pe_manifest_absent_requested_execution_level" },
			{ absent_requested_execution_ui_access,
				"pe_manifest_absent_requested_execution_ui_access" },
			{ absent_msix_publisher, "pe_manifest_absent_msix_publisher" },
			{ absent_msix_package_name, "pe_manifest_absent_msix_package_name" },
			{ absent_msix_application_id, "pe_manifest_absent_msix_application_id" },
			{ multiple_msix_elements, "pe_manifest_multiple_msix_elements" },
			{ empty_progids, "pe_manifest_empty_progids" },
			{ empty_window_class, "pe_manifest_empty_window_class" },
			{ pe_bliss::resources::manifest_loader_errc::absent_declaration,
				"pe_manifest_absent_declaration" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void manifest_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<manifest_format_rule>(shared_values);
}

} //namespace bv::pe
