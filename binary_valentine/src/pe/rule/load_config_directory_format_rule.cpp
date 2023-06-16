#include "binary_valentine/pe/rule/load_config_directory_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/load_config/load_config_directory.h"
#include "pe_bliss2/load_config/load_config_directory_loader.h"

namespace bv::pe
{

class load_config_directory_format_rule final
	: public core::rule_base<load_config_directory_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_load_config_directory_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		load_config_directory_format_rule,
		pe_report::load_config_directory_format_error,
		pe_report::invalid_load_config_directory_size>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::load_config::load_config_directory_details& header) const
	{
		std::visit([&reporter](const auto& dir) {
			check_format_errors(reporter, dir);
			check_load_config_version(reporter, dir);
		}, header.get_value());
	}

private:
	template<typename Reporter, typename Directory>
	static void check_load_config_version(Reporter& reporter, const Directory& dir)
	{
		if (!dir.version_exactly_matches())
		{
			// If not the last known version...
			if (dir.get_version() != pe_bliss::load_config::version::memcpy_guard)
				reporter.template log<pe_report::invalid_load_config_directory_size>();
		}
	}

	template<typename Reporter, typename Directory>
	static void check_format_errors(Reporter& reporter, const Directory& dir)
	{
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, dir, error_code_to_message_id_);

		if (dir.get_guard_cf_function_table())
		{
			error_helpers::report_errors<pe_report::load_config_directory_format_error>(
				reporter, *dir.get_guard_cf_function_table(), error_code_to_message_id_);
		}

		if (dir.get_volatile_metadata())
		{
			error_helpers::report_errors<pe_report::load_config_directory_format_error>(
				reporter, *dir.get_volatile_metadata(), error_code_to_message_id_);
		}

		if (dir.get_enclave_config())
		{
			error_helpers::report_errors<pe_report::load_config_directory_format_error>(
				reporter, *dir.get_enclave_config(), error_code_to_message_id_);
			error_helpers::report_errors<pe_report::load_config_directory_format_error>(
				reporter, dir.get_enclave_config()->get_imports(), error_code_to_message_id_);
		}

		std::visit([&reporter](const auto& chpe_metadata) {
			check_chpe_format_errors(reporter, chpe_metadata);
		}, dir.get_chpe_metadata());

		if (dir.get_dynamic_relocation_table())
		{
			error_helpers::report_errors<pe_report::load_config_directory_format_error>(
				reporter, *dir.get_dynamic_relocation_table(), error_code_to_message_id_);

			std::visit([&reporter](const auto& table) {
				check_dynamic_reloc_table_format_errors(reporter, table);
			}, dir.get_dynamic_relocation_table()->get_relocations());
		}
	}

	template<typename Reporter>
	static constexpr void check_dynamic_reloc_table_format_errors(Reporter&, std::monostate) {}

	template<typename Reporter, typename Pointer>
	static void check_dynamic_reloc_table_format_errors(Reporter& reporter,
		const std::vector<pe_bliss::load_config::dynamic_relocation_table_v1_details<Pointer>>& tables)
	{
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, tables, error_code_to_message_id_);

		pe_bliss::error_list merged;
		for (const auto& table : tables)
		{
			std::visit([&merged](const auto& fixup_lists) {
				merge_dvrtv1_header_format_errors(fixup_lists, merged);
				error_helpers::merge_errors(fixup_lists, merged);
			}, table.get_fixup_lists());
		}
		
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, merged, error_code_to_message_id_);
	}

	template<typename Relocs>
	static constexpr void merge_dvrtv1_header_format_errors(const Relocs&,
		const pe_bliss::error_list&)
	{
	}

	static void merge_dvrtv1_header_format_errors(
		const pe_bliss::load_config::dynamic_relocation_table_base_v1_base<pe_bliss::error_list>
			::arm64x_dynamic_relocation_list_type& relocs,
		pe_bliss::error_list& merged)
	{
		for (const auto& fixup_list : relocs)
		{
			for (const auto& fixup : fixup_list.get_fixups())
			{
				std::visit([&merged](const auto& arm64x_fixup) {
					using fixup_type = std::remove_reference_t<decltype(arm64x_fixup)>;
					if constexpr (std::is_base_of_v<pe_bliss::error_list, fixup_type>)
						error_helpers::merge_errors(arm64x_fixup, merged);
				}, fixup);
			}
		}
	}

	template<typename Reporter, typename Pointer>
	static void check_dynamic_reloc_table_format_errors(Reporter& reporter,
		const std::vector<pe_bliss::load_config::dynamic_relocation_table_v2_details<Pointer>>& tables)
	{
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, tables, error_code_to_message_id_);

		pe_bliss::error_list merged;
		for (const auto& table : tables)
		{
			std::visit([&merged](const auto& header) {
				merge_dvrtv2_header_format_errors(header, merged);
			}, table.get_header());

			error_helpers::merge_errors(table.get_fixup_lists(), merged);
		}

		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, merged, error_code_to_message_id_);
	}

	static void constexpr merge_dvrtv2_header_format_errors(std::monostate,
		pe_bliss::error_list&) {}

	static void constexpr merge_dvrtv2_header_format_errors(
		const pe_bliss::load_config::prologue_dynamic_relocation_header&,
		pe_bliss::error_list&) {}

	static void merge_dvrtv2_header_format_errors(
		const pe_bliss::load_config::epilogue_dynamic_relocation_header_details& header,
		pe_bliss::error_list& merged)
	{
		error_helpers::merge_errors(header, merged);
	}

	template<typename Reporter>
	static constexpr void check_chpe_format_errors(Reporter&, std::monostate) {}

	template<typename Reporter, typename Chpe>
	static void check_chpe_format_errors(Reporter& reporter, const Chpe& metadata)
	{
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, metadata, error_code_to_message_id_);
		error_helpers::report_errors<pe_report::load_config_directory_format_error>(
			reporter, metadata.get_range_entries(), error_code_to_message_id_);
	}

	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::load_config::load_config_directory_loader_errc;
		return {
			{ invalid_dynamic_relocation_table_section_index,
				"pe_load_config_invalid_dynamic_relocation_table_section_index" },
			{ invalid_dynamic_relocation_table_section_offset,
				"pe_load_config_invalid_dynamic_relocation_table_section_offset" },
			{ unknown_dynamic_relocation_table_version,
				"pe_load_config_unknown_dynamic_relocation_table_version" },
			{ invalid_lock_prefix_table,
				"pe_load_config_invalid_lock_prefix_table" },
			{ invalid_safeseh_handler_table,
				"pe_load_config_invalid_safeseh_handler_table" },
			{ invalid_cf_function_table,
				"pe_load_config_invalid_cf_function_table" },
			{ invalid_cf_export_suppression_table,
				"pe_load_config_invalid_cf_export_suppression_table" },
			{ invalid_cf_longjump_table, "pe_load_config_invalid_cf_longjump_table" },
			{ unknown_chpe_metadata_type, "pe_load_config_unknown_chpe_metadata_type" },
			{ invalid_chpe_metadata, "pe_load_config_invalid_chpe_metadata" },
			{ invalid_chpe_range_entries, "pe_load_config_invalid_chpe_range_entries" },
			{ invalid_dynamic_relocation_table,
				"pe_load_config_invalid_dynamic_relocation_table" },
			{ invalid_dynamic_relocation_table_v2_size,
				"pe_load_config_invalid_dynamic_relocation_table_v2_size" },
			{ invalid_dynamic_relocation_table_v1_size,
				"pe_load_config_invalid_dynamic_relocation_table_v1_size" },
			{ invalid_dynamic_relocation_header_size,
				"pe_load_config_invalid_dynamic_relocation_header_size" },
			{ invalid_dynamic_relocation_fixup_info_size,
				"pe_load_config_invalid_dynamic_relocation_fixup_info_size" },
			{ invalid_dynamic_relocation_prologue,
				"pe_load_config_invalid_dynamic_relocation_prologue" },
			{ invalid_dynamic_relocation_epilogue,
				"pe_load_config_invalid_dynamic_relocation_epilogue" },
			{ unknown_dynamic_relocation_symbol,
				"pe_load_config_unknown_dynamic_relocation_symbol" },
			{ invalid_dynamic_relocation_block_size,
				"pe_load_config_invalid_dynamic_relocation_block_size" },
			{ unaligned_dynamic_relocation_block,
				"pe_load_config_unaligned_dynamic_relocation_block" },
			{ unknown_arm64x_relocation_type,
				"pe_load_config_unknown_arm64x_relocation_type" },
			{ invalid_arm64x_dynamic_relocation_copy_data_size,
				"pe_load_config_invalid_arm64x_dynamic_relocation_copy_data_size" },
			{ invalid_arm64x_dynamic_relocation_add_delta_size,
				"pe_load_config_invalid_arm64x_dynamic_relocation_add_delta_size" },
			{ invalid_base_relocation_size, "pe_load_config_invalid_base_relocation_size" },
			{ invalid_dynamic_relocation_size, "pe_load_config_invalid_dynamic_relocation_size" },
			{ invalid_cf_guard_table_size, "pe_load_config_invalid_cf_guard_table_size" },
			{ unsorted_cf_guard_table, "pe_load_config_unsorted_cf_guard_table" },
			{ invalid_dynamic_relocation_epilogue_size,
				"pe_load_config_invalid_dynamic_relocation_epilogue_size" },
			{ invalid_dynamic_relocation_epilogue_branch_descriptor_size,
				"pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptor_size" },
			{ invalid_dynamic_relocation_epilogue_branch_descriptors,
				"pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptors" },
			{ invalid_dynamic_relocation_epilogue_branch_descriptor_bit_map,
				"pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptor_bit_map" },
			{ invalid_enclave_config, "pe_load_config_invalid_enclave_config" },
			{ invalid_enclave_config_extra_data,
				"pe_load_config_invalid_enclave_config_extra_data" },
			{ invalid_enclave_import_extra_data,
				"pe_load_config_invalid_enclave_import_extra_data" },
			{ invalid_enclave_import_name, "pe_load_config_invalid_enclave_import_name" },
			{ invalid_volatile_metadata, "pe_load_config_invalid_volatile_metadata" },
			{ unaligned_volatile_metadata_access_rva_table_size,
				"pe_load_config_unaligned_volatile_metadata_access_rva_table_size" },
			{ unaligned_volatile_metadata_range_table_size,
				"pe_load_config_unaligned_volatile_metadata_range_table_size" },
			{ invalid_volatile_metadata_access_rva_table,
				"pe_load_config_invalid_volatile_metadata_access_rva_table" },
			{ invalid_volatile_metadata_range_table,
				"pe_load_config_invalid_volatile_metadata_range_table" },
			{ invalid_ehcont_targets, "pe_load_config_invalid_ehcont_targets" },
			{ unsorted_ehcont_targets, "pe_load_config_unsorted_ehcont_targets" },
			{ invalid_xfg_type_based_hash_rva,
				"pe_load_config_invalid_xfg_type_based_hash_rva" },
			{ invalid_func_override_size, "pe_load_config_invalid_func_override_size" },
			{ invalid_func_override_rvas_size,
				"pe_load_config_invalid_func_override_rvas_size" },
			{ invalid_func_override_fixup, "pe_load_config_invalid_func_override_fixup" },
			{ invalid_func_override_dynamic_relocation,
				"pe_load_config_invalid_func_override_dynamic_relocation" },
			{ invalid_func_override_rvas, "pe_load_config_invalid_func_override_rvas" },
			{ invalid_bdd_info_size, "pe_load_config_invalid_bdd_info_size" },
			{ invalid_bdd_info_entry, "pe_load_config_invalid_bdd_info_entry" },
			{ invalid_bdd_dynamic_relocations,
				"pe_load_config_invalid_bdd_dynamic_relocations" },
			{ unknown_bdd_info_entry_version,
				"pe_load_config_unknown_bdd_info_entry_version" },
			{ invalid_load_config_directory,
				"pe_load_config_invalid_load_config_directory" },
			{ invalid_guard_memcpy_function_pointer_va,
				"pe_load_config_invalid_guard_memcpy_function_pointer_va" },
			{ invalid_cast_guard_os_determined_failure_mode_va,
				"pe_load_config_invalid_cast_guard_os_determined_failure_mode_va" },
			{ invalid_guard_xfg_check_function_pointer_va,
				"pe_load_config_invalid_guard_xfg_check_function_pointer_va" },
			{ invalid_guard_xfg_dispatch_function_pointer_va,
				"pe_load_config_invalid_guard_xfg_dispatch_function_pointer_va" },
			{ invalid_guard_xfg_table_dispatch_function_pointer_va,
				"pe_load_config_invalid_guard_xfg_table_dispatch_function_pointer_va" },
			{ invalid_enclave_import_array,
				"pe_load_config_invalid_enclave_import_array" },
			{ invalid_dynamic_relocation_entry,
				"pe_load_config_invalid_dynamic_relocation_entry" },
			{ invalid_base_relocation, "pe_load_config_invalid_base_relocation" },
			{ invalid_dynamic_relocation_block,
				"pe_load_config_invalid_dynamic_relocation_block" },
			{ invalid_arm64x_relocation_entry,
				"pe_load_config_invalid_arm64x_relocation_entry" },
			{ invalid_arm64x_dynamic_relocation_copy_data_data,
				"pe_load_config_invalid_arm64x_dynamic_relocation_copy_data_data" },
			{ invalid_arm64x_dynamic_relocation_add_delta_entry,
				"pe_load_config_invalid_arm64x_dynamic_relocation_add_delta_entry" },
			{ invalid_security_cookie_va, "pe_load_config_invalid_security_cookie_va" },
			{ invalid_cf_guard_table_function_count,
				"pe_load_config_invalid_cf_guard_table_function_count" },
			{ invalid_guard_export_suppression_table_size,
				"pe_load_config_invalid_guard_export_suppression_table_size" },
			{ invalid_guard_export_suppression_table_function_count,
				"pe_load_config_invalid_guard_export_suppression_table_function_count" },
			{ unsorted_guard_export_suppression_table,
				"pe_load_config_unsorted_guard_export_suppression_table" },
			{ invalid_guard_longjump_table_size,
				"pe_load_config_invalid_guard_longjump_table_size" },
			{ invalid_guard_longjump_table_function_count,
				"pe_load_config_invalid_guard_longjump_table_function_count" },
			{ unsorted_guard_longjump_table,
				"pe_load_config_unsorted_guard_longjump_table" },
			{ invalid_guard_cf_check_function_va,
				"pe_load_config_invalid_guard_cf_check_function_va" },
			{ invalid_guard_cf_dispatch_function_va,
				"pe_load_config_invalid_guard_cf_dispatch_function_va" },
			{ invalid_chpe_range_entry_count,
				"pe_load_config_invalid_chpe_range_entry_count" },
			{ invalid_chpe_entry_address_or_size,
				"pe_load_config_invalid_chpe_entry_address_or_size" },
			{ invalid_volatile_metadata_access_rva_table_entry_count,
				"pe_load_config_invalid_volatile_metadata_access_rva_table_entry_count" },
			{ invalid_volatile_metadata_range_table_entry_count,
				"pe_load_config_invalid_volatile_metadata_range_table_entry_count" },
			{ invalid_ehcont_target_rvas, "pe_load_config_invalid_ehcont_target_rvas" },
			{ invalid_ehcont_targets_count, "pe_load_config_invalid_ehcont_targets_count" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void load_config_directory_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<load_config_directory_format_rule>(shared_values);
}

} //namespace bv::pe
