#pragma once

#include "binary_valentine/output/rule_report.h"

namespace bv::pe_report
{

//debug_directory_rule
using coff_mpdb_debug_directory_present = output::rule_report_info<
	"PE001", output::report_category::security, output::report_level::warning>;
using debug_directory_present = output::rule_report_info<
	"PE002", output::report_category::security, output::report_level::info>;
using mpx_debug_directory_present = output::rule_report_info<
	"PE003", output::report_category::configuration, output::report_level::info>;
using pogo_optimization_pgi_status = output::rule_report_info<
	"PE055", output::report_category::optimization, output::report_level::warning>;

//export_rule
using export_library_name_mismatch = output::rule_report_info<
	"PE004", output::report_category::configuration, output::report_level::info>;
using non_dll_exports_functions = output::rule_report_info<
	"PE005", output::report_category::configuration, output::report_level::info>;

//pe_file_format_rule
using exe_has_dll_flag = output::rule_report_info<
	"PE006", output::report_category::configuration, output::report_level::warning>;
using dll_has_no_dll_flag = output::rule_report_info<
	"PE023", output::report_category::configuration, output::report_level::warning>;

//relocs_rule
using relocs_stripped = output::rule_report_info<
	"PE007", output::report_category::security, output::report_level::error>;
using relocs_section_not_discardable = output::rule_report_info<
	"PE008", output::report_category::optimization, output::report_level::warning>;
using dynamic_base_disabled = output::rule_report_info<
	"PE056", output::report_category::security, output::report_level::error>;

//safeseh_rule
using no_safeseh = output::rule_report_info<
	"PE009", output::report_category::security, output::report_level::error>;

//sections_rule
using rwx_section_present = output::rule_report_info<
	"PE010", output::report_category::security, output::report_level::critical>;
using wx_section_present = output::rule_report_info<
	"PE011", output::report_category::security, output::report_level::error>;
using sw_section_present = output::rule_report_info<
	"PE012", output::report_category::security, output::report_level::warning>;
using writable_rdata_section = output::rule_report_info<
	"PE117", output::report_category::security, output::report_level::warning>;
using executable_rdata_section = output::rule_report_info<
	"PE118", output::report_category::security, output::report_level::error>;

//security_cookie_rule
using no_security_cookie = output::rule_report_info<
	"PE013", output::report_category::security, output::report_level::critical>;
using no_security_cookie_for_some_objects = output::rule_report_info<
	"PE014", output::report_category::security, output::report_level::error>;
using security_cookie_value_not_default = output::rule_report_info<
	"PE015", output::report_category::security, output::report_level::error>;

//simple_flags_rule
using not_large_address_aware = output::rule_report_info<
	"PE016", output::report_category::security, output::report_level::warning>;
using not_large_address_aware_no_high_entropy_va = output::rule_report_info<
	"PE018", output::report_category::security, output::report_level::warning>;
using no_high_entropy_va = output::rule_report_info<
	"PE057", output::report_category::security, output::report_level::warning>;
using dep_disabled = output::rule_report_info<
	"PE019", output::report_category::security, output::report_level::critical>;
using signature_check_not_forced = output::rule_report_info<
	"PE021", output::report_category::security, output::report_level::warning>;
using aslr_compatibility_mode = output::rule_report_info<
	"PE022", output::report_category::security, output::report_level::error>;
using not_signed = output::rule_report_info<
	"PE017", output::report_category::security, output::report_level::error>;
using not_terminal_server_aware = output::rule_report_info<
	"PE099", output::report_category::configuration, output::report_level::info>;

//version_info_rule
using version_info_absent = output::rule_report_info<
	"PE024", output::report_category::configuration, output::report_level::warning>;
using version_info_inferred = output::rule_report_info<
	"PE025", output::report_category::configuration, output::report_level::error>;
using version_info_non_release = output::rule_report_info<
	"PE026", output::report_category::configuration, output::report_level::warning>;
using version_info_debug = output::rule_report_info<
	"PE065", output::report_category::configuration, output::report_level::warning>;
using version_info_private_build = output::rule_report_info<
	"PE027", output::report_category::configuration, output::report_level::warning>;
using version_info_string_file_info_absent = output::rule_report_info<
	"PE034", output::report_category::configuration, output::report_level::error>;
using version_info_string_file_info_lcid_resource_lang_mismatch = output::rule_report_info<
	"PE035", output::report_category::configuration, output::report_level::warning>;
using version_info_translations_absent = output::rule_report_info<
	"PE036", output::report_category::configuration, output::report_level::error>;
using version_info_translation_lcid_resource_lang_mismatch = output::rule_report_info<
	"PE037", output::report_category::configuration, output::report_level::warning>;
using version_info_translations_strings_mismatch = output::rule_report_info<
	"PE038", output::report_category::configuration, output::report_level::warning>;
using version_info_private_build_absent = output::rule_report_info<
	"PE049", output::report_category::configuration, output::report_level::warning>;
using version_info_special_build_absent = output::rule_report_info<
	"PE050", output::report_category::configuration, output::report_level::warning>;
using version_info_private_build_present = output::rule_report_info<
	"PE051", output::report_category::configuration, output::report_level::warning>;
using version_info_special_build_present = output::rule_report_info<
	"PE052", output::report_category::configuration, output::report_level::warning>;
using version_info_fixed_file_version_string_file_version_mismatch = output::rule_report_info<
	"PE053", output::report_category::configuration, output::report_level::warning>;
using version_info_fixed_product_version_string_product_version_mismatch = output::rule_report_info<
	"PE054", output::report_category::configuration, output::report_level::warning>;
using version_info_version_string_not_parsable = output::rule_report_info<
	"PE066", output::report_category::configuration, output::report_level::info>;
using version_info_version_string_has_tail = output::rule_report_info<
	"PE067", output::report_category::configuration, output::report_level::info>;
using version_info_cross_string_presence_mismatch = output::rule_report_info<
	"PE068", output::report_category::configuration, output::report_level::warning>;
using version_info_cross_string_file_info_mismatch = output::rule_report_info<
	"PE069", output::report_category::configuration, output::report_level::warning>;
using version_info_incorrect_file_type = output::rule_report_info<
	"PE100", output::report_category::configuration, output::report_level::warning>;
using version_info_default = output::rule_report_info<
	"PE122", output::report_category::configuration, output::report_level::error>;
using version_info_original_filename_mismatch = output::rule_report_info<
	"PE164", output::report_category::configuration, output::report_level::info>;
using version_info_required_string_absent = output::rule_report_info<
	"PE042", output::report_category::configuration, output::report_level::warning>;
using version_info_required_string_empty = output::rule_report_info<
	"PE043", output::report_category::configuration, output::report_level::warning>;

//cross_resource_version_info_rule
using version_info_multiple_resources_multiple_translations = output::rule_report_info<
	"PE039", output::report_category::configuration, output::report_level::warning>;
using version_info_cross_resource_string_file_info_mismatch = output::rule_report_info<
	"PE040", output::report_category::configuration, output::report_level::warning>;
using version_info_fixed_file_info_mismatch = output::rule_report_info<
	"PE028", output::report_category::configuration, output::report_level::warning>;
using version_info_cross_resource_string_presence_mismatch = output::rule_report_info<
	"PE029", output::report_category::configuration, output::report_level::warning>;

//flow_guards_rule
using guard_cf_disabled = output::rule_report_info<
	"PE020", output::report_category::security, output::report_level::error>;
using guard_cf_enabled_no_dynamic_base = output::rule_report_info<
	"PE063", output::report_category::security, output::report_level::error>;
using guard_rf_enabled_deprecated = output::rule_report_info<
	"PE058", output::report_category::security, output::report_level::info>;
using guard_cf_delay_load_unprotected = output::rule_report_info<
	"PE059", output::report_category::security, output::report_level::warning>;
using guard_cf_delay_load_iat_not_marked_in_separate_section = output::rule_report_info<
	"PE060", output::report_category::security, output::report_level::info>;
using delay_load_iat_not_in_separate_section = output::rule_report_info<
	"PE061", output::report_category::security, output::report_level::info>;
using guard_xf_disabled = output::rule_report_info<
	"PE062", output::report_category::security, output::report_level::info>;
using guard_eh_disabled = output::rule_report_info<
	"PE064", output::report_category::security, output::report_level::warning>;
using guard_eh_continuation_table_not_readonly = output::rule_report_info<
	"PE123", output::report_category::security, output::report_level::error>;
using guard_cf_function_table_not_readonly = output::rule_report_info<
	"PE124", output::report_category::security, output::report_level::error>;
using guard_cf_address_taken_iat_entry_table_not_readonly = output::rule_report_info<
	"PE125", output::report_category::security, output::report_level::error>;
using guard_cf_long_jump_target_table_not_readonly = output::rule_report_info<
	"PE126", output::report_category::security, output::report_level::error>;
using guard_retpoline_disabled = output::rule_report_info<
	"PE127", output::report_category::security, output::report_level::info>;

//cross_resource_version_info_rule
using version_info_invalid_translation_format = output::rule_report_info<
	"PE030", output::report_category::format, output::report_level::critical>;
using version_info_invalid_resource_format = output::rule_report_info<
	"PE031", output::report_category::format, output::report_level::critical>;
using version_info_duplicate_block = output::rule_report_info<
	"PE032", output::report_category::format, output::report_level::critical>;
using version_info_duplicate_translations = output::rule_report_info<
	"PE033", output::report_category::format, output::report_level::critical>;
using version_info_duplicate_strings = output::rule_report_info<
	"PE041", output::report_category::format, output::report_level::critical>;
using version_info_duplicate_string_languages = output::rule_report_info<
	"PE070", output::report_category::format, output::report_level::critical>;
using version_info_invalid_string_translations = output::rule_report_info<
	"PE071", output::report_category::format, output::report_level::critical>;
using version_info_invalid_block_format = output::rule_report_info<
	"PE072", output::report_category::format, output::report_level::critical>;

//main_icon_rule
using no_main_icon = output::rule_report_info<
	"PE073", output::report_category::configuration, output::report_level::info>;
using main_icon_height_width_mismatch = output::rule_report_info<
	"PE074", output::report_category::configuration, output::report_level::error>;
using main_icon_duplicate_size = output::rule_report_info<
	"PE075", output::report_category::configuration, output::report_level::error>;
using main_icon_missing_size = output::rule_report_info<
	"PE076", output::report_category::configuration, output::report_level::warning>;
using main_icon_too_low_bit_count = output::rule_report_info<
	"PE077", output::report_category::configuration, output::report_level::warning>;
using multilingual_main_icon = output::rule_report_info<
	"PE078", output::report_category::configuration, output::report_level::warning>;
using main_icon_format_error = output::rule_report_info<
	"PE079", output::report_category::format, output::report_level::critical>;

//cet_rule
using cet_not_enabled = output::rule_report_info<
	"PE080", output::report_category::security, output::report_level::warning>;

//not_recommended_import_rule
using dangerous_winapi_import = output::rule_report_info<
	"PE081", output::report_category::security, output::report_level::warning>;
using deprecated_winapi_import = output::rule_report_info<
	"PE082", output::report_category::system, output::report_level::warning>;
using insecure_winapi_import = output::rule_report_info<
	"PE083", output::report_category::security, output::report_level::error>;
using internal_winapi_import = output::rule_report_info<
	"PE084", output::report_category::system, output::report_level::error>;
using dangerous_winapi_delay_import = output::rule_report_info<
	"PE085", output::report_category::security, output::report_level::warning>;
using deprecated_winapi_delay_import = output::rule_report_info<
	"PE086", output::report_category::system, output::report_level::warning>;
using insecure_winapi_delay_import = output::rule_report_info<
	"PE087", output::report_category::security, output::report_level::error>;
using internal_winapi_delay_import = output::rule_report_info<
	"PE088", output::report_category::system, output::report_level::error>;
using xbox_only_winapi_import = output::rule_report_info<
	"PE089", output::report_category::system, output::report_level::error>;
using xbox_only_winapi_delay_import = output::rule_report_info<
	"PE090", output::report_category::system, output::report_level::error>;

//ansi_import_rule
using ansi_import = output::rule_report_info<
	"PE091", output::report_category::system, output::report_level::warning>;
using ansi_delay_import = output::rule_report_info<
	"PE092", output::report_category::system, output::report_level::warning>;

//directory_sections_rule
using executable_export_directory = output::rule_report_info<
	"PE093", output::report_category::security, output::report_level::error>;
using writable_export_directory = output::rule_report_info<
	"PE094", output::report_category::security, output::report_level::warning>;
using executable_import_directory = output::rule_report_info<
	"PE095", output::report_category::security, output::report_level::error>;
using writable_import_directory = output::rule_report_info<
	"PE096", output::report_category::security, output::report_level::warning>;
using executable_exception_directory = output::rule_report_info<
	"PE097", output::report_category::security, output::report_level::error>;
using writable_exception_directory = output::rule_report_info<
	"PE098", output::report_category::security, output::report_level::warning>;
using executable_relocation_directory = output::rule_report_info<
	"PE101", output::report_category::security, output::report_level::error>;
using writable_relocation_directory = output::rule_report_info<
	"PE102", output::report_category::security, output::report_level::warning>;
using executable_debug_directory = output::rule_report_info<
	"PE103", output::report_category::security, output::report_level::error>;
using writable_debug_directory = output::rule_report_info<
	"PE104", output::report_category::security, output::report_level::warning>;
using executable_tls_directory = output::rule_report_info<
	"PE105", output::report_category::security, output::report_level::error>;
using writable_tls_directory = output::rule_report_info<
	"PE106", output::report_category::security, output::report_level::warning>;
using executable_config_directory = output::rule_report_info<
	"PE107", output::report_category::security, output::report_level::error>;
using writable_config_directory = output::rule_report_info<
	"PE108", output::report_category::security, output::report_level::warning>;
using executable_bound_import_directory = output::rule_report_info<
	"PE109", output::report_category::security, output::report_level::error>;
using writable_bound_import_directory = output::rule_report_info<
	"PE110", output::report_category::security, output::report_level::warning>;
using executable_delay_import_directory = output::rule_report_info<
	"PE111", output::report_category::security, output::report_level::error>;
using writable_delay_import_directory = output::rule_report_info<
	"PE112", output::report_category::security, output::report_level::warning>;
using executable_iat_directory = output::rule_report_info<
	"PE113", output::report_category::security, output::report_level::error>;
using writable_entry_point = output::rule_report_info<
	"PE114", output::report_category::security, output::report_level::error>;

//checksum_rule
using absent_checksum = output::rule_report_info<
	"PE115", output::report_category::security, output::report_level::warning>;
using incorrect_checksum = output::rule_report_info<
	"PE116", output::report_category::security, output::report_level::critical>;

//rich_data_rule
using tool_potential_preview_version = output::rule_report_info<
	"PE119", output::report_category::security, output::report_level::info>;
using tool_preview_version = output::rule_report_info<
	"PE120", output::report_category::security, output::report_level::warning>;
using too_old_product_version = output::rule_report_info<
	"PE121", output::report_category::security, output::report_level::warning>;

//manifest_rule
using manifest_does_not_exist = output::rule_report_info<
	"PE128", output::report_category::configuration, output::report_level::error>;
using manifest_exists_not_isolated = output::rule_report_info<
	"PE129", output::report_category::system, output::report_level::error>;
using external_and_embedded_manifests_exist = output::rule_report_info<
	"PE130", output::report_category::configuration, output::report_level::warning>;
using manifest_uac_virtualization_enabled = output::rule_report_info<
	"PE131", output::report_category::system, output::report_level::info>;
using manifest_win10_win11_server2016_server2019_server2022_not_supported = output::rule_report_info<
	"PE132", output::report_category::system, output::report_level::warning>;
using manifest_gaps_in_supported_os_versions = output::rule_report_info<
	"PE133", output::report_category::system, output::report_level::warning>;
using manifest_no_heap_type = output::rule_report_info<
	"PE134", output::report_category::optimization, output::report_level::info>;
using manifest_not_long_path_aware = output::rule_report_info<
	"PE135", output::report_category::system, output::report_level::info>;
using manifest_unknown_heap_type = output::rule_report_info<
	"PE136", output::report_category::optimization, output::report_level::info>;
using manifest_absent_supported_os_list = output::rule_report_info<
	"PE137", output::report_category::system, output::report_level::warning>;
using manifest_no_printer_driver_isolation = output::rule_report_info<
	"PE138", output::report_category::system, output::report_level::info>;
using manifest_auto_elevate_present = output::rule_report_info<
	"PE139", output::report_category::system, output::report_level::info>;
using manifest_dpi_unaware_no_gdi_scaling = output::rule_report_info<
	"PE142", output::report_category::system, output::report_level::info>;
using manifest_dpi_unaware_with_gdi_scaling = output::rule_report_info<
	"PE143", output::report_category::system, output::report_level::info>;
using manifest_dpi_per_monitor_v2_unaware = output::rule_report_info<
	"PE144", output::report_category::system, output::report_level::info>;
using manifest_ui_access_not_signed = output::rule_report_info<
	"PE160", output::report_category::system, output::report_level::error>;

//image_errors_rule
using image_load_error = output::rule_report_info<
	"PE140", output::report_category::format, output::report_level::critical>;
using image_load_warning = output::rule_report_info<
	"PE141", output::report_category::format, output::report_level::warning>;

//manifest_format_rule
using manifest_load_error = output::rule_report_info<
	"PE145", output::report_category::format, output::report_level::error>;
using manifest_format_error = output::rule_report_info<
	"PE146", output::report_category::format, output::report_level::error>;
using app_manifest_unsupported_element = output::rule_report_info<
	"PE149", output::report_category::format, output::report_level::warning>;
using assembly_manifest_unsupported_element = output::rule_report_info<
	"PE150", output::report_category::format, output::report_level::warning>;

//load_config_flags_rule
using executable_process_heap = output::rule_report_info<
	"PE147", output::report_category::security, output::report_level::error>;
using global_flags_overridden = output::rule_report_info<
	"PE148", output::report_category::security, output::report_level::warning>;

//dotnet_header_format_rule
using dotnet_header_format_error = output::rule_report_info<
	"PE151", output::report_category::format, output::report_level::error>;

//debug_directory_format_rule
using debug_directory_format_error = output::rule_report_info<
	"PE152", output::report_category::format, output::report_level::error>;

//import_format_rule
using import_directory_format_error = output::rule_report_info<
	"PE153", output::report_category::format, output::report_level::error>;
using delay_import_directory_format_error = output::rule_report_info<
	"PE154", output::report_category::format, output::report_level::error>;

//export_directory_format_rule
using export_directory_format_error = output::rule_report_info<
	"PE155", output::report_category::format, output::report_level::error>;

//load_config_directory_format_rule
using load_config_directory_format_error = output::rule_report_info<
	"PE156", output::report_category::format, output::report_level::error>;
using invalid_load_config_directory_size = output::rule_report_info<
	"PE157", output::report_category::format, output::report_level::info>;

//resource_directory_format_rule
using resource_directory_format_error = output::rule_report_info<
	"PE158", output::report_category::format, output::report_level::error>;
using resource_directory_loop = output::rule_report_info<
	"PE159", output::report_category::format, output::report_level::warning>;

//vc_feature_rule
using sdl_not_always_used = output::rule_report_info<
	"PE161", output::report_category::security, output::report_level::warning>;
using pre_cpp11_used = output::rule_report_info<
	"PE162", output::report_category::security, output::report_level::warning>;

//manifest_assembly_version_rule
using manifest_assembly_version_version_info_version_mismatch = output::rule_report_info<
	"PE163", output::report_category::configuration, output::report_level::warning>;

//combined_version_info_rule
using version_info_fixed_product_version_cross_image_mismatch = output::rule_report_info<
	"PE044", output::report_category::configuration, output::report_level::warning>;
using version_info_translations_cross_image_mismatch = output::rule_report_info<
	"PE045", output::report_category::configuration, output::report_level::warning>;
using version_info_strings_cross_image_mismatch = output::rule_report_info<
	"PE046", output::report_category::configuration, output::report_level::warning>;

//NEXT: PE165, PE047, PE048
} //namespace bv::pe_report
