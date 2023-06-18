#include "binary_valentine/pe/rule/application_manifest_rule.h"

#include <array>
#include <cstddef>
#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/import_based_info.h"
#include "binary_valentine/pe/data/manifest_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/resources/manifest.h"

namespace bv::pe
{

class application_manifest_rule final
	: public core::rule_base<application_manifest_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_application_manifest_rule";
	static constexpr auto reports = output::get_rule_reports<
		application_manifest_rule,
		pe_report::manifest_does_not_exist,
		pe_report::manifest_exists_not_isolated,
		pe_report::external_and_embedded_manifests_exist,
		pe_report::manifest_uac_virtualization_enabled,
		pe_report::manifest_absent_supported_os_list,
		pe_report::manifest_win10_win11_server2016_server2019_server2022_not_supported,
		pe_report::manifest_gaps_in_supported_os_versions,
		pe_report::manifest_no_heap_type,
		pe_report::manifest_not_long_path_aware,
		pe_report::manifest_unknown_heap_type,
		pe_report::manifest_no_printer_driver_isolation,
		pe_report::manifest_auto_elevate_present,
		pe_report::manifest_dpi_unaware_no_gdi_scaling,
		pe_report::manifest_dpi_unaware_with_gdi_scaling,
		pe_report::manifest_dpi_per_monitor_v2_unaware,
		pe_report::manifest_ui_access_not_signed>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code
			&& !info.is_pre_win7ce && !info.is_boot
			&& !info.is_driver
			&& info.file_extension == ".exe";
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::image::image& image,
		const manifest_info& manifest,
		const import_based_info* import_based) const
	{
		if (manifest.type == manifest_type::no_manifest)
		{
			reporter.template log<pe_report::manifest_does_not_exist>();
			return;
		}

		if (manifest.type != manifest_type::application)
			return;

		if (image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::no_isolation)
		{
			reporter.template log<pe_report::manifest_exists_not_isolated>();
			return;
		}

		check_multiple_manifests(reporter, manifest);

		if (!manifest.manifest)
			return;

		const auto& manifest_obj = manifest.manifest.value();
		check_uac_virtualization(reporter, manifest_obj, image);
		check_supported_os_versions(reporter, manifest_obj);
		check_heap_type(reporter, manifest_obj);
		check_long_path_aware(reporter, manifest_obj);
		check_printer_driver_isolation(reporter, manifest_obj);
		check_auto_elevate(reporter, manifest_obj);

		if (image.get_optional_header().is_windows_gui())
			check_dpi_aware(reporter, manifest_obj, import_based);
	}

private:
	template<typename Reporter>
	static void check_multiple_manifests(Reporter& reporter,
		const manifest_info& manifest)
	{
		if (manifest.external_result == external_manifest_load_result::exists
			&& manifest.has_embedded_manifest)
		{
			reporter.template log<pe_report::external_and_embedded_manifests_exist>();
		}
	}

	template<typename Reporter>
	static void check_dpi_aware(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest,
		const import_based_info* import_based)
	{
		if (import_based && import_based->is_service)
			return;

		bool is_aware = false;
		bool is_per_monitor_v2_aware = false;
		const auto dpi_awareness = manifest.get_dpi_awareness().get_dpi_awareness_value();
		if (dpi_awareness == pe_bliss::resources::dpi_awareness_value::absent)
		{
			const auto dpi_aware = manifest.get_dpi_awareness().get_dpi_aware_value();
			if (dpi_aware != pe_bliss::resources::dpi_aware_value::absent
				&& dpi_aware != pe_bliss::resources::dpi_aware_value::dpi_unaware)
			{
				is_aware = true;
			}
		}
		else if (dpi_awareness != pe_bliss::resources::dpi_awareness_value::dpi_unaware
			&& dpi_awareness != pe_bliss::resources::dpi_awareness_value::unrecognized)
		{
			is_aware = true;
			is_per_monitor_v2_aware = dpi_awareness
				== pe_bliss::resources::dpi_awareness_value::dpi_aware_per_monitor_v2;
		}

		if (!is_aware)
		{
			if (!manifest.gdi_scaling())
				reporter.template log<pe_report::manifest_dpi_unaware_no_gdi_scaling>();
			else
				reporter.template log<pe_report::manifest_dpi_unaware_with_gdi_scaling>();
		}
		else if (!is_per_monitor_v2_aware)
		{
			reporter.template log<pe_report::manifest_dpi_per_monitor_v2_unaware>();
		}
	}

	template<typename Reporter>
	static void check_uac_virtualization(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest,
		const pe_bliss::image::image& image)
	{
		if (!manifest.get_requested_privileges())
		{
			reporter.template log<pe_report::manifest_uac_virtualization_enabled>();
		}
		else
		{
			if (manifest.get_requested_privileges()->get_ui_access().value_or(false)
				&& !image.get_data_directories().has_security())
			{
				reporter.template log<pe_report::manifest_ui_access_not_signed>();
			}
		}
	}

	template<typename Reporter>
	static void check_auto_elevate(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (manifest.auto_elevate())
			reporter.template log<pe_report::manifest_auto_elevate_present>();
	}

	template<typename Reporter>
	static void check_printer_driver_isolation(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (!manifest.printer_driver_isolation())
			reporter.template log<pe_report::manifest_no_printer_driver_isolation>();
	}

	template<typename Reporter>
	static void check_heap_type(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (!manifest.get_heap_type())
		{
			reporter.template log<pe_report::manifest_no_heap_type>();
			return;
		}

		if (manifest.get_heap_type()->get_heap_type()
			== pe_bliss::resources::heap_type_value::unknown)
		{
			reporter.template log<pe_report::manifest_unknown_heap_type>(
				output::named_arg("type", manifest.get_heap_type()->get_heap_type_raw()));
		}
	}

	template<typename Reporter>
	static void check_long_path_aware(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		if (!manifest.long_path_aware())
			reporter.template log<pe_report::manifest_not_long_path_aware>();
	}

	template<typename Reporter>
	static void check_supported_os_versions(Reporter& reporter,
		const pe_bliss::resources::native_manifest_details& manifest)
	{
		const auto& list = manifest.get_supported_os_list();
		if (!list)
		{
			reporter.template log<pe_report::manifest_absent_supported_os_list>();
			return;
		}

		using enum pe_bliss::resources::assembly_supported_os;
		std::array<bool,
			static_cast<std::size_t>(win10_win11_server2016_server2019_server2022)
			- static_cast<std::size_t>(win_vista_server2008) + 1u> supported_os{};
		for (auto os : list->get_list())
		{
			auto os_int = static_cast<std::size_t>(os);
			if (os_int < static_cast<std::size_t>(win_vista_server2008)
				|| os_int > static_cast<std::size_t>(
					win10_win11_server2016_server2019_server2022))
			{
				continue;
			}

			supported_os[os_int] = true;
		}

		if (!supported_os[static_cast<std::size_t>(
			win10_win11_server2016_server2019_server2022)])
		{
			reporter.template log<
				pe_report::manifest_win10_win11_server2016_server2019_server2022_not_supported>();
		}

		auto last_true = std::find(supported_os.rbegin(), supported_os.rend(), true);
		if (last_true == supported_os.rend())
			return;

		auto first_true = std::find(supported_os.begin(), supported_os.end(), true);
		auto last_true_base = last_true.base();
		if (std::find(first_true, last_true_base, false) != last_true_base)
			reporter.template log<pe_report::manifest_gaps_in_supported_os_versions>();
	}
};

void application_manifest_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<application_manifest_rule>(shared_values);
}

} //namespace bv::pe
