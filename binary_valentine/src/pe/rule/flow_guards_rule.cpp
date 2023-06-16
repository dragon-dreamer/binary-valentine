#include "binary_valentine/pe/rule/flow_guards_rule.h"

#include <string_view>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/helpers/section_helpers.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/core/optional_header.h"
#include "pe_bliss2/delay_import/delay_import_directory.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/image_section_search.h"
#include "pe_bliss2/load_config/load_config_directory.h"
#include "pe_bliss2/section/section_header.h"

namespace bv::pe
{

class flow_guards_rule final
	: public core::rule_base<flow_guards_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_flow_guards_rule";
	static constexpr auto reports = output::get_rule_reports<
		flow_guards_rule,
		pe_report::guard_cf_disabled,
		pe_report::guard_cf_enabled_no_dynamic_base,
		pe_report::guard_eh_disabled,
		pe_report::guard_rf_enabled_deprecated,
		pe_report::guard_cf_delay_load_unprotected,
		pe_report::guard_cf_delay_load_iat_not_marked_in_separate_section,
		pe_report::delay_load_iat_not_in_separate_section,
		pe_report::guard_xf_disabled,
		pe_report::guard_eh_continuation_table_not_readonly,
		pe_report::guard_cf_function_table_not_readonly,
		pe_report::guard_cf_address_taken_iat_entry_table_not_readonly,
		pe_report::guard_cf_long_jump_target_table_not_readonly,
		pe_report::guard_retpoline_disabled>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code
			&& !info.is_boot && !info.is_il_only && !info.is_mixed;
	}

	static bool has_basic_cf_guard(
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		return std::visit([](const auto& dir) {
			if (!dir.is_version_at_least(pe_bliss::load_config::version::cf_guard))
				return false;

			using enum pe_bliss::load_config::guard_flags::value;
			if ((dir.get_guard_flags() & (cf_instrumented | cf_function_table_present))
				!= (cf_instrumented | cf_function_table_present))
			{
				return false;
			}

			const auto& cf_guard_info = dir.get_descriptor()->cf_guard;
			if (!cf_guard_info.guard_cf_check_function_pointer
				|| !cf_guard_info.guard_cf_dispatch_function_pointer
				|| !cf_guard_info.guard_cf_function_table
				|| !cf_guard_info.guard_cf_function_count)
			{
				return false;
			}

			return true;
		}, load_config.get_value());
	}

	static bool is_delay_iat_cf_protected(
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		return std::visit([](const auto& dir) {
			using enum pe_bliss::load_config::guard_flags::value;
			return (dir.get_guard_flags() & protect_delayload_iat)
				&& dir.is_version_at_least(pe_bliss::load_config::version::cf_guard_ex);
		}, load_config.get_value());
	}

	static bool is_delay_iat_marked_in_separate_section(
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		return std::visit([](const auto& dir) {
			using enum pe_bliss::load_config::guard_flags::value;
			return (dir.get_guard_flags() & delayload_iat_in_its_own_section)
				&& dir.is_version_at_least(pe_bliss::load_config::version::cf_guard_ex);
		}, load_config.get_value());
	}

	template<typename Reporter, typename ImportLib>
	static void check_delay_load_iat_in_separate_section(
		Reporter& reporter,
		const pe_bliss::image::image& image,
		const ImportLib& lib)
	{
		auto [iat_section, iat_data] = pe_bliss::image::section_from_rva(image,
			lib.get_descriptor()->address_table, 1u);

		if (iat_section == image.get_section_table().get_section_headers().end())
		{
			reporter.log(output::report_level::error,
				"unable_to_check_delay_load_iat");
			return;
		}

		using enum pe_bliss::section::section_header::characteristics::value;
		if ((iat_section->get_characteristics()
			& (mem_execute | mem_read | mem_write | mem_shared))
			!= (mem_read | mem_write))
		{
			reporter.template log<
				pe_report::delay_load_iat_not_in_separate_section>(
					output::named_arg("library_name", lib.get_library_name().value()));
			return;
		}

		if (section_helpers::count_directories_in_section(iat_section, image) != 0u)
		{
			reporter.template log<
				pe_report::delay_load_iat_not_in_separate_section>(
				output::named_arg("library_name", lib.get_library_name().value()));
		}
	}

	template<typename Reporter>
	static void check_delay_load_iat_in_separate_section(
		Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::delay_import::delay_import_directory_details* delay_import)
	{
		if (!delay_import)
			return;

		std::visit([&reporter, &image](const auto& list) {
			for (const auto& lib : list)
			{
				try
				{
					check_delay_load_iat_in_separate_section(reporter, image, lib);
				}
				catch (...)
				{
					reporter.log(output::report_level::error,
						"unable_to_check_delay_load_iat");
				}
			}
		}, delay_import->get_list());
	}

	template<typename Report, typename Reporter>
	static void check_in_readonly_memory(Reporter& reporter,
		std::uint64_t va, const pe_bliss::image::image& image)
	{
		if (!va)
			return;

		auto [section, data] = pe_bliss::image::section_from_va(image, va, 1u);
		if (section == image.get_section_table().get_section_headers().end())
			return;

		using enum pe_bliss::section::section_header::characteristics::value;
		if ((section->get_characteristics()
			& (mem_execute | mem_read | mem_write | mem_shared)) != mem_read)
		{
			reporter.template log<Report>();
		}
	}

	template<typename Reporter>
	static void check_cf_guard_tables(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		std::visit([&reporter, &image](const auto& dir) {
			if (dir.is_version_at_least(pe_bliss::load_config::version::eh_guard))
			{
				check_in_readonly_memory<pe_report::guard_eh_continuation_table_not_readonly>(
					reporter,
					dir.get_descriptor()->guard_exception_handling.guard_eh_continuation_table,
					image);
			}

			if (dir.is_version_at_least(pe_bliss::load_config::version::cf_guard))
			{
				check_in_readonly_memory<pe_report::guard_cf_function_table_not_readonly>(
					reporter, dir.get_descriptor()->cf_guard.guard_cf_function_table, image);
			}

			if (dir.is_version_at_least(pe_bliss::load_config::version::cf_guard_ex))
			{
				check_in_readonly_memory<pe_report::guard_cf_address_taken_iat_entry_table_not_readonly>(
					reporter, dir.get_descriptor()->cf_guard_ex.guard_address_taken_iat_entry_table,
					image);
				check_in_readonly_memory<pe_report::guard_cf_long_jump_target_table_not_readonly>(
					reporter, dir.get_descriptor()->cf_guard_ex.guard_long_jump_target_table, image);
			}

		}, load_config.get_value());
	}

	template<typename Reporter>
	static void check_cf_guard(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config,
		const pe_bliss::delay_import::delay_import_directory_details* delay_import)
	{
		if (!load_config ||
			!(image.get_optional_header().get_dll_characteristics()
				& pe_bliss::core::optional_header::dll_characteristics::guard_cf)
			|| !has_basic_cf_guard(*load_config))
		{
			reporter.template log<pe_report::guard_cf_disabled>();
			return;
		}

		if (!(image.get_optional_header().get_dll_characteristics()
			& pe_bliss::core::optional_header::dll_characteristics::dynamic_base))
		{
			reporter.template log<pe_report::guard_cf_enabled_no_dynamic_base>();
		}

		check_cf_guard_tables(reporter, image, *load_config);

		if (image.get_data_directories().has_delay_import())
		{
			if (!is_delay_iat_cf_protected(*load_config))
			{
				reporter.template log<pe_report::guard_cf_delay_load_unprotected>();
			}
			else
			{
				if (!is_delay_iat_marked_in_separate_section(*load_config))
				{
					reporter.template log<
						pe_report::guard_cf_delay_load_iat_not_marked_in_separate_section>();
				}
				else
				{
					check_delay_load_iat_in_separate_section(reporter, image, delay_import);
				}
			}
		}

		if (has_exception_handlers(image))
		{
			bool has_ehcont_targets = std::visit([](const auto& dir) {
				using enum pe_bliss::load_config::guard_flags::value;
				return (dir.get_guard_flags()
						& (eh_continuation_table_present_20h1 | eh_continuation_table_present))
					&& dir.is_version_at_least(pe_bliss::load_config::version::eh_guard)
					&& dir.get_descriptor()->guard_exception_handling.guard_eh_continuation_table;
			}, load_config->get_value());

			if (!has_ehcont_targets)
				reporter.template log<pe_report::guard_eh_disabled>();
		}
	}

	static bool has_exception_handlers(const pe_bliss::image::image& image)
	{
		return image.is_64bit()
			&& image.get_data_directories().has_exception_directory();
	}

	static bool has_rf_guard(
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		return std::visit([](const auto& dir) {
			if (!dir.is_version_at_least(pe_bliss::load_config::version::rf_guard))
				return false;

			using enum pe_bliss::load_config::guard_flags::value;
			return ((dir.get_guard_flags() & (rf_enable | rf_instrumented | rf_strict)) != 0u);
		}, load_config.get_value());
	}

	template<typename Reporter>
	static void check_rf_guard(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config)
	{
		if (!load_config)
			return;

		if (has_rf_guard(*load_config))
			reporter.template log<pe_report::guard_rf_enabled_deprecated>();
	}

	static bool has_basic_xf_guard(
		const pe_bliss::load_config::load_config_directory_details& load_config)
	{
		return std::visit([](const auto& dir) {
			if (!dir.is_version_at_least(pe_bliss::load_config::version::xf_guard))
				return false;

			if (!(dir.get_guard_flags() & pe_bliss::load_config::guard_flags::xfg_enabled))
				return false;

			const auto& xf_guard_info = dir.get_descriptor()->extended_flow_guard;
			if (!xf_guard_info.guard_xfg_check_function_pointer
				|| !xf_guard_info.guard_xfg_dispatch_function_pointer
				|| !xf_guard_info.guard_xfg_table_dispatch_function_pointer)
			{
				return false;
			}

			return true;
		}, load_config.get_value());
	}

	template<typename Reporter>
	static void check_xf_guard(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config)
	{
		if (!image.is_64bit())
			return;

		if (!load_config ||
			!(image.get_optional_header().get_dll_characteristics()
				& pe_bliss::core::optional_header::dll_characteristics::guard_cf)
			|| !has_basic_xf_guard(*load_config))
		{
			reporter.template log<pe_report::guard_xf_disabled>();
			return;
		}
	}

	template<typename Reporter>
	static void check_retpoline(Reporter& reporter,
		const pe_bliss::load_config::load_config_directory_details* load_config,
		const basic_pe_info& info)
	{
		if (!info.is_driver)
			return;

		if (!load_config)
		{
			reporter.template log<pe_report::guard_retpoline_disabled>();
			return;
		}

		const bool has_retpoline = std::visit([](const auto& dir) {
			using enum pe_bliss::load_config::guard_flags::value;
			return (dir.get_guard_flags() & retpoline_present)
				&& dir.is_version_at_least(pe_bliss::load_config::version::rf_guard);
		}, load_config->get_value());

		if (!has_retpoline)
			reporter.template log<pe_report::guard_retpoline_disabled>();
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const basic_pe_info& info,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config,
		const pe_bliss::delay_import::delay_import_directory_details* delay_import) const
	{
		check_cf_guard(reporter, image, load_config, delay_import);
		check_rf_guard(reporter, image, load_config);
		check_xf_guard(reporter, image, load_config);
		check_retpoline(reporter, load_config, info);
	}
};

void flow_guards_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<flow_guards_rule>(shared_values);
}

} //namespace bv::pe
