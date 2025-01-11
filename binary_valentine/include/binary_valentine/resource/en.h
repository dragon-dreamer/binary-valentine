#pragma once

#include <array>
#include <string_view>
#include <utility>

#include "binary_valentine/output/internal_report_messages.h"

namespace bv::resource::en
{

constexpr auto strings = std::to_array<std::pair<std::string_view, std::string_view>>({
	{ "pe_debug_directory_generator", "PE debug directory parser" },
	{ "pe_export_directory_generator", "PE export directory parser" },
	{ "pe_load_config_directory_generator", "PE load configuration directory parser" },
	{ "pe_resource_directory_generator", "PE resource directory parser" },
	{ "pe_version_info_generator", "PE version info resource parser" },
	{ "program_path_generator", "Entity path generator" },
	{ "pe_basic_dotnet_info_generator", "PE basic .NET info parser" },
	{ "basic_pe_info_generator", "PE basic info generator" },
	{ "pe_delay_import_directory_generator", "PE delay import directory parser" },
	{ "pe_icon_info_generator", "PE icon info parser" },
	{ "pe_import_directory_generator", "PE import directory parser" },
	{ "pe_rich_header_generator", "PE rich header parser" },
	{ "pe_image_generator", "PE image parser" },
	{ "pe_import_based_info_generator", "PE import based info generator" },
	{ "pe_authenticode_generator", "PE Authenticode signature generator" },
	{ "pe_security_directory_generator", "PE security directory generator" },

	{ "pe_full_winapi_map_generator", "PE full WinAPI map loader" },
	{ "pe_not_recommended_imports_generator", "PE not recommended WinAPI loader" },
	
	{ "report_category_system", "System" },
	{ "report_category_optimization", "Optimization" },
	{ "report_category_security", "Security" },
	{ "report_category_configuration", "Configuration" },
	{ "report_category_format", "Format" },
	{ "report_category_unknown", "Unknown" },

	{ "report_level_info", "INFO" },
	{ "report_level_warning", "WARNING" },
	{ "report_level_error", "ERROR" },
	{ "report_level_critical", "CRITICAL" },
	{ "report_level_unknown", "UNKNOWN" },

	{ output::reports::absent_text, "(absent)" },
	{ output::reports::rule_exception, "Exception in rule {rule_name}: {exception}" },
	{ output::reports::generator_exception, "Exception in generator {generator_name}: {exception}" },
	{ output::reports::no_generator_for_type, "No generator for type {tag_name}" },
	{ output::reports::inaccessible_entity, "Inaccessible file or directory '{entity_name}': {exception}" },
	{ output::reports::system_error_description,
		"System error: category: '{error_category_name}', code '{error_code_value}', '{error_message}'" },
	{ output::reports::user_error_description, "User error: {error_message}" },
	{ output::reports::exception_description, "Exception: '{error_message}'" },
	{ output::reports::analysis_exception, "Analysis exception: {exception}" },
	{ output::reports::entity_load_exception, "Entity '{entity_name}' load exception: {exception}" },
	{ output::reports::entity_processing_error, "Entity processing exception: {exception}" },
	{ output::reports::description_not_available, "Description is not available" },
	{ output::reports::target_enumeration_error, "Target enumeration error: {exception}" },
	{ output::reports::explicit_stop_requested, "Analysis stop requested, aborting early" },
	{ output::reports::analysis_finished_message, "Analysis complete in {duration} sec. "
		"Found issues: {entity_issues}. Analysis errors: {analysis_issues}." },
	{ output::reports::analysis_interrupted_message, "Analysis was interrupted in {duration} sec. "
		"Found issues: {entity_issues}. Analysis errors: {analysis_issues}." },
	{ output::reports::writing_report_error, "Error writing output report '{path}': {exception}" },

	{ output::reports::progress_analysis_completed, "(PROGRESS) Analysis completed"},
	{ output::reports::progress_analysis_started, "(PROGRESS) Analysis started"},
	{ output::reports::progress_loaded, "(PROGRESS) Target loaded"},
	{ output::reports::progress_load_started, "(PROGRESS) Target load started"},
	{ output::reports::progress_combined_analysis_started, "(PROGRESS) Combined analysis started"},
	{ output::reports::progress_combined_analysis_completed, "(PROGRESS) Combined analysis completed"},
	{ output::reports::progress_target_skipped_unsupported,
		"(PROGRESS) Target analysis skipped (format not supported)"},
	{ output::reports::progress_target_skipped_filtered,
		"(PROGRESS) Target skipped (filtered)"},
	
	//user_error error codes
	{ "user_error_unable_to_read_file", "Unable to read file" },
	{ "user_error_invalid_regex", "Invalid ECMAScript regex: '{regex}'"},
	{ "user_error_invalid_root_path", "Invalid root path" },
	{ "user_error_invalid_thread_count", "Invalid thread count: '{value}'" },
	{ "user_error_ambiguous_preload_limit_settings", "Ambiguous preload limit settings "
		"(both 'max_concurrent_tasks' and 'max_loaded_targets_size' are present)" },
	{ "user_error_invalid_max_concurrent_tasks_value", "Invalid max_concurrent_tasks value ('{value}'). "
		"It should be greater than zero and greater than or equal to the thread_count (if set explicitly)" },
	{ "user_error_invalid_max_loaded_targets_size_value", "Invalid max_loaded_targets_size value ('{value}'). "
		"It should be a non-zero numerical value with the suffix ('B' for bytes, 'K' for kilobytes, " 
		"'M' for megabytes or 'G' for gigabytes)" },
	{ "user_error_invalid_combined_analysis_value", "Invalid combined_analysis value ('{value}'), "
		"expected boolean value ('true', 'false', '1', or '0')" },
	{ "user_error_multiple_xml_elements", "Multiple XML tags '{tag}' when at most one is expected" },
	{ "user_error_unsupported_selection_mode", "Unsupported selection mode ('{mode}'), "
		"expected 'include' or 'exclude'" },
	{ "user_error_unsupported_report_id", "Unsupported report ID ('{report_id}')" },
	{ "user_error_duplicate_report_id", "Duplicate report ID ('{report_id}')" },
	{ "user_error_unsupported_report_level", "Unsupported report level ('{level}'). "
		"Expected 'info', 'warning', 'error', or 'critical'" },
	{ "user_error_unsupported_report_category", "Unsupported report category ('{category}'). "
		"Expected 'system', 'optimization', 'security', 'configuration', or 'format'" },
	{ "user_error_absent_report_filter_report_attribute",
		"Required 'report' attribute is absent in report_filter" },
	{ "user_error_duplicate_report_filter", "Duplicate filtered report for report ID '{filter}'" },
	{ "user_error_duplicate_included_excluded_reports",
		"More than one 'include_reports' or 'exclude_reports' option is present. "
		"At most one is allowed for a rule selector"},
	{ "user_error_unsupported_report_list_tag", "Unsupported tag in 'include_reports'/'exclude_reports' node. "
		"Only 'report' is supported. Encountered tag: '{tag}'" },
	{ "user_error_duplicate_excluded_levels", "Duplicate 'exclude_levels' tag in the rule selector" },
	{ "user_error_unsupported_excluded_levels_list_tag", "Unsupported tag in 'exclude_levels' node. "
		"Only 'level' is supported. Encountered tag: '{tag}'" },
	{ "user_error_duplicate_excluded_level", "Duplicate excluded level in the rule selector: '{level}'" },
	{ "user_error_duplicate_excluded_categories", "Duplicate 'exclude_categories' tag in the rule selector" },
	{ "unsupported_excluded_categories_list_tag", "Unsupported tag in 'exclude_categories' node. "
		"Only 'category' is supported. Encountered tag: '{tag}'" },
	{ "user_error_duplicate_excluded_category",
		"Duplicate excluded category in the rule selector: '{category}'" },
	{ "user_error_unsupported_rule_selector_tag", "Unsupported tag in the rule selector node. "
		"Only 'include_reports', 'exclude_reports', 'exclude_levels', 'exclude_categories', "
		"'report_filter' are supported. Encountered tag: '{tag}'" },
	{ "user_error_unsupported_aggregation_mode", "Unsupported 'aggregation_mode': '{mode}'. "
		"Expected: 'all' or 'any'" },
	{ "user_error_unsupported_report_filter_tag", "Unsupported tag in 'report_filter' node. "
		"Only 'regex' is supported. Encountered tag: '{tag}'" },
	{ "user_error_absent_report_filter_arg", "'report_filter' node 'regex' "
		"does not have the required 'arg' attribute" },
	{ "user_error_invalid_report_filter", "Invalid 'report_filter' ECMAScript regex: '{regex}'" },
	{ "user_error_no_targets_specified", "Required 'targets' node is absent or empty. "
		"At least one target should be specified for analysis" },
	{ "user_error_invalid_recursive_attribute_value", "Invalid 'recursive' attribute value ('{value}'), "
		"expected boolean value ('true', 'false', '1', or '0')" },
	{ "user_error_unsupported_targets_tag", "Unsupported tag in 'targets' node. "
		"Only 'target' is supported. Encountered tag: '{tag}'" },
	{ "user_error_absent_target_path", "No target path is specified" },
	{ "user_error_unsupported_filter_tag", "Unsupported tag in 'filter' node. "
		"Only 'include' and 'exclude' are supported. Encountered tag: '{tag}'" },
	{ "user_error_invalid_target_filter", "Invalid 'include' or 'exclude' ECMAScript regex: '{regex}'" },
	{ "user_error_unsupported_reports_tag", "Unsupported tag in 'reports' node. "
		"Only 'terminal', 'text', 'sarif' and 'html' are supported. Encountered tag: '{tag}'" },
	{ "user_error_empty_report_path", "Empty output report path" },
	{ "user_error_unable_to_load_xml", "Unable to load XML. Underlying error: '{error}'"},
	{ "user_error_no_reports_specified", "Reports are absent or empty. "
		"At least one output report should be specified" },
	{ "user_error_no_options_allowed_with_config", "No additional options are allowed "
		"when --config is specified." },
	{ "user_error_invalid_target_path", "Invalid executable target path." },
	{ "user_error_invalid_report_path", "Invalid output report file path." },
	{ "user_error_invalid_project_path", "Invalid project path." },
	{ "user_error_unable_to_write_file", "Unable to write file. Underlying error: '{error}'" },
	{ "user_error_duplicate_output_extra_option", "Duplicate output report extra option name: '{option}'" },
	{ "user_error_unable_to_load_html_report_template_file",
		"Unable to load HTML report template file: '{file}'" },
	{ "user_error_unable_to_write_html_report",
		"Unable to write HTML report. Underlying error message: '{underlying}'" },
		
	{ "unable_to_load_analysis_plan", "Unable to load analysis plan: {exception}." },

	{ "pe_debug_directory_rule", "PE debug directory rule" },
	{ "PE001", "COFF/MPDB debug directory is present" },
	{ "PE001_description", "COFF/MPDB (Portable PDB) debug directory is present. "
		"These directories may contain embedded symbols which will ease reverse engineering." },
	{ "PE002", "Debug directory is present" },
	{ "PE002_description", "Debug directory is present. "
		"This may ease reverse engineering or analysis of an executable." },
	{ "PE003", "MPX debug directory is present" },
	{ "PE003_description", "MPX (Intel Memory Protection Extensions) debug directory is present. "
		"This directory is deprecated and should not be present in modern executables." },
	{ "PE055", "Profile-guided optimization is not finalized" },
	{ "pogo_optimization_pgi_status", "Profile-guided optimization is enabled for the executable, "
		"but its current status is 'instrument'. This means that the profile information is being collected, "
		"but was not used to optimize this executable." },

	{ "pe_export_rule", "PE export directory rule" },
	{ "PE004", "Export library name mismatch" },
	{ "PE004_description", "Export directory contains a library name ({library_name}). "
		"This name does not match the name of the executable file ({file_name})." },
	{ "PE005", "Non-DLL executable exports symbols" },
	{ "PE005_description", "The executable file is not marked as a dynamic loaded library, "
		"but has exported symbols. This is an allowed, but non-standard solution." },

	{ "pe_file_format_rule", "PE file format rule" },
	{ "PE006", "EXE file has DLL flag set" },
	{ "PE006_description", "The file with the '{extension}' extension has the DLL flag set "
		"in the file header. This does not make sense." },
	{ "PE023", "Dynamic load library does not have DLL flag set" },
	{ "PE023_description", "The file with the '{extension}' extension does not have the DLL flag set "
		"in the file header. This does not make sense." },

	{ "pe_relocs_rule", "PE relocations rule" },
	{ "PE007", "Relocations are absent" },
	{ "PE007_description", "Relocations are absent in the executable. "
		"This makes the executable more vulnerable to exploits. ASLR security mitigation "
		"is not available for executables without relocations." },
	{ "PE056", "Dynamic base is disabled" },
	{ "PE056_description", "Dynamic image base is disabled. "
		"This prevents the loader from relocating the executable and "
		"makes the executable more vulnerable to exploits. ASLR security mitigation "
		"is not available for executables without dynamic base." },
	{ "PE008", "Relocations section is not discardable" },
	{ "PE008_description", "Relocations executable section ('{section_name}') is not marked discardable. "
		"This will lead to increased memory consumption, as the loader will not be able to free "
		"the memory used by the relocation directory after it has been processed."},

	{ "pe_safeseh_rule", "PE SAFESEH rule" },
	{ "PE009", "SAFESEH is disabled" },
	{ "PE009_description", "SAFESEH is disabled for the executable which uses SEH (structured exception handling). "
		"This makes it easier to exploit memory corruption vulnerabilities by taking control of the "
		"execution flow through exception handling mechanisms." },

	{ "pe_sections_rule", "PE sections rule" },
	{ "PE010", "RWX section is present" },
	{ "PE010_description", "RWX section is present ('{section_name}'). "
		"A section which is marked readable, writable and executable at the same time "
		"makes vulnerability exploitation much easier, allowing the attacker to "
		"write the data to memory and execute it straightaway."},
	{ "PE011", "WX section is present" },
	{ "PE011_description", "WX section is present ('{section_name}'). "
		"A section which is marked writable and executable at the same time "
		"makes vulnerability exploitation easier. It may provide a target for "
		"writing the shellcode with subsequent execution."},
	{ "PE012", "SW section is present" },
	{ "PE012_description", "SW section is present ('{section_name}'). "
		"A section which is writable and shared at the same time "
		"will share writable memory between processes. This may permit a low privileged process "
		"access memory of a high privileged one." },
	{ "PE117", "Writable .rdata section" },
	{ "PE117_description", "The executable contains the '.rdata' section which is marked writable. "
		"This section is typically dedicated to read-only data and thus should be placed to read-only memory." },
	{ "PE118", "Executable .rdata section" },
	{ "PE118_description", "The executable contains the '.rdata' section which is marked executable. "
		"This section is typically dedicated to read-only data and thus should be placed to read-only memory." },

	{ "pe_security_cookie_rule", "PE security cookie rule" },
	{ "PE013", "Security cookie is absent" },
	{ "PE013_description", "Stack protection security cookie is absent. "
		"This makes it easier to exploit stack memory corruption vulnerabilities by taking control of the "
		"execution flow when returning from a function call." },
	{ "PE014", "Security cookie is absent for some object files" },
	{ "PE014_description", "Stack protection security cookie is absent for some object files. "
		"Object files with security cookie enabled: {gs_count}. "
		"Total number of C/C++ object files: {c_and_c_plus_plus_count}. "
		"This makes it easier to exploit vulnerabilities by taking control of the "
		"execution flow when returning from a function call." },
	{ "PE015", "Security cookie value is not system default" },
	{ "PE015_description", "Stack protection security cookie value is not system default. "
		"Current value: '{cookie_value:#x}'."
		"This prevents the loader from replacing its value with a value from "
		"a secure random source, which may make it more predictable for an attacker. "
		"This makes it easier to exploit vulnerabilities by taking control of the "
		"execution flow when returning from a function call." },

	{ "pe_simple_flags_rule", "PE headers flags rule" },
	{ "PE016", "Not marked as large address aware" },
	{ "PE016_description", "The executable is not marked large address aware. "
		"This limits the address space to 3Gb only and makes some exploitation "
		"techniques easier. This also disables 64-bit ASLR (address space layout randomization) "
		"even though it is enabled for the executable." },
	{ "PE018", "High entropy ASLR compatibility disabled, not large address aware" },
	{ "PE018_description", "The executable is not marked large address aware, "
		"which limits the address space to 3Gb only and makes some exploitation "
		"techniques easier. The executable also has 64-bit ASLR (address space layout randomization) "
		"disabled. High entropy ASLR is more effective in mitigating memory corruption vulnerabilities." },
	{ "PE057", "High entropy ASLR compatibility disabled" },
	{ "PE057_description", "The executable has 64-bit ASLR (address space layout randomization) "
		"disabled. High entropy ASLR is more effective in mitigating memory corruption vulnerabilities." },
	{ "PE019", "DEP disabled" },
	{ "PE019_description", "DEP (data execution prevention) is disabled. "
		"This allows self-modifying code (including exploits) to be executed easily "
		"even from the write-only memory without the need to manipulate memory attributes." },
	{ "PE022", "ASLR compatibility mode is active" },
	{ "PE022_description", "ASLR (address space layout randomization) compatibility mode is active, "
		"as the executable image base is set to the value less than 0xFFFFFFFF (current image base: '{image_base:#x}'). "
		"This limits ASLR in mitigating memory corruption vulnerabilities."},
	{ "PE099", "Executable is not terminal server aware" },
	{ "PE099_description", "Executable is not marked as terminal server aware. "
		"If an executable is launched under the terminal server session, Terminal Server makes certain modifications "
		"to the application to make it work properly in a multiuser environment. For example, Terminal Server creates "
		"a virtual Windows folder, such that each user gets a Windows folder instead of getting the system's Windows directory." },
			
	{ "pe_flow_guards_rule", "PE flow guards rule" },
	{ "PE020", "CF guard is disabled" },
	{ "PE020_description", "Control flow guard is disabled. "
		"This makes it easier to exploit memory corruption vulnerabilities by taking "
		"control of the execution flow, because the flow is not being checked in any way." },
	{ "PE063", "CF guard is enabled, but dynamic base is disabled" },
	{ "PE063_description", "Control flow guard is enabled, but dynamic base is not. "
		"Dynamic base is a prerequisite for the control flow guard." },
	{ "PE058", "RF guard is enabled" },
	{ "PE058_description", "Return flow guard is enabled. "
		"This technology had been experimental and did not materialize in the later Windows 10/11 versions."
		"It is recommended to turn off the return flow guard to prevent the linker "
		"from generating excessive unused metadata for the image."},
	{ "PE059", "CF guard is enabled, but delay load IAT is not protected" },
	{ "PE059_description", "Control flow guard is enabled, and the executable uses delay loaded libraries. "
		"The delay load import address table (IAT) is not protected by the control flow guard."},
	{ "PE060", "Delay load IAT is protected by CF guard, but is not marked as being in the separate section" },
	{ "PE060_description", "Control flow guard is enabled, and the executable uses delay loaded libraries. "
		"The delay load import address table (IAT) is protected by the control flow guard, "
		"but is not marked as being placed in a separate section. "
		"Having delay load IAT in a separate read-write section is recommended to keep compatibility "
		"with older non-CFG aware operating system versions."},
	{ "PE061", "Delay load IAT is protected by CF guard, but is not in the separate section" },
	{ "PE061_description", "Control flow guard is enabled, and the executable uses delay loaded libraries. "
		"The delay load import address table (IAT) is protected by the control flow guard, "
		"but is not placed in a separate read-write section. "
		"Having delay load IAT in a separate read-write section is recommended to keep compatibility "
		"with older non-CFG aware operating system versions. "
		"Affected IAT import library name: '{library_name}'."},
	{ "PE062", "XF guard is disabled" },
	{ "PE062_description", "Extended flow guard is disabled. This guard is an extended version of "
		"CFG (control flow guard) with stricter hash-based call target checking. "
		"XFG makes capturing the execution flow more difficult for an attacker."},
	{ "PE064", "EH guard is disabled" },
	{ "PE064_description", "Exception handling control flow guard is disabled. This guard is an extended version of "
		"CFG (control flow guard), which creates a list of valid exception handling continuation targets for an executable. "
		"EH guard makes capturing the execution flow more difficult for an attacker." },
	{ "unable_to_check_delay_load_iat", "Unable to check delay load IAT for the executable." },
	{ "PE123", "EH Guard continuation table not readonly" },
	{ "PE123_description", "Exception handling control flow guard continuation table is not "
		"placed in a read-only section ('{section}'). Section must not be writable, executable, or shared." },
	{ "PE124", "CF Guard function table not readonly" },
	{ "PE124_description", "Control Flow guard function table is not "
		"placed in a read-only section ('{section}'). Section must not be writable, executable, or shared." },
	{ "PE125", "CF Guard address taken IAT entry table not readonly" },
	{ "PE125_description", "Control Flow guard address taken IAT entry table is not "
		"placed in a read-only section ('{section}'). Section must not be writable, executable, or shared." },
	{ "PE126", "CF Guard long jump target table not readonly" },
	{ "PE126_description", "Control Flow guard long jump target table is not "
		"placed in a read-only section ('{section}'). Section must not be writable, executable, or shared." },
	{ "PE127", "Retpoline guard is not enabled" },
	{ "PE127_description", "Retpoline guard is not enabled for the kernel mode driver. "
		"It may make sense to enable the retpoline mitigation for some kernel mode drivers." },
		
	{ "pe_version_info_rule", "PE version info rule" },
	{ "PE024", "Version info is absent" },
	{ "PE024_description", "Version information is absent in the executable. "
		"The file lacks any user-readable description. "
		"This may also make antivirus software more suspicious when analyzing such executables." },
	{ "PE026", "The file is marked as a development version, not a commercially released product" },
	{ "PE026_description", "Version information contains the VS_FF_PRERELEASE flag which indicates that "
		"the executable is a development version, not a commercially released product. "
		"Make sure you are not distributing development product versions. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE065", "The file is marked as a debug version" },
	{ "PE065_description", "Version information contains the VS_FF_DEBUG flag which indicates that "
		"the executable contains debugging information or is compiled with debugging features enabled. "
		"Make sure you are not distributing development product versions. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE025", "The file version structure was created dynamically" },
	{ "PE025_description", "Version information contains the VS_FF_INFOINFERRED flag which indicates that "
		"the version structure was created dynamically; "
		"therefore, some of the members in this structure may be empty or incorrect. "
		"This flag should never be set. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE027", "The file is marked as non-standard release (private build)" },
	{ "PE027_description", "Version information contains the VS_FF_PRIVATEBUILD flag which indicates that "
		"the executable was not built using standard release procedures. "
		"Make sure you are not distributing development product versions. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE034", "Version info lacks string file info" },
	{ "PE034_description", "Version information does not contain any StringFileInfo blocks. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE035", "Version info StringFileInfo block language does not match resource language" },
	{ "PE035_description", "Version information contains a StringFileInfo block with the language and character set ID. "
		"Version information itself is placed in the executable resource directory under the entry with particular language ID. "
		"In this executable, StringFileInfo language ID does not match the corresponding resource language ID. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'. "
		"Affected StringFileInfo language: {string_file_info_lang}, tag: '{string_file_info_lang_tag}', "
		"location: '{string_file_info_lang_location}'." },
	{ "PE036", "Version info VarFileInfo block lacks translation info" },
	{ "PE036_description", "Version information does not contain translation info. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE037", "Version info VarFileInfo block translation language does not match resource language" },
	{ "PE037_description", "Version information contains a VarFileInfo block with translation the language and character set ID. "
		"Version information itself is placed in the executable resource directory under the entry with particular language ID. "
		"In this executable, VarFileInfo translation language ID does not match the corresponding resource language ID. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'. "
		"Affected VarFileInfo language: {translations_lang}, tag: '{translations_lang_tag}', "
		"location: '{translations_lang_location}'." },
	{ "PE038", "Version info StringFileInfo block languages do not match VarFileInfo translation languages" },
	{ "PE038_description", "Version information contains StringFileInfo blocks with language and character set IDs. "
		"Version information also contains VarFileInfo blocks with the corresponding translation language and character set IDs. "
		"In this executable, StringFileInfo languages do not match VarFileInfo translation languages. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE042", "Required StringFileInfo string is absent" },
	{ "PE042_description", "Version information StringFileInfo block lacks a required string. "
		"Affected string key: '{key}'. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE043", "Required StringFileInfo string is empty" },
	{ "PE043_description", "Version information StringFileInfo block has a required string with the empty value. "
		"Affected string key: '{key}'. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE049", "Version info StringFileInfo lacks PrivateBuild string" },
	{ "PE049_description", "Version information StringFileInfo block lacks the PrivateBuild string, which is required when "
		"version info has the VS_FF_PRIVATEBUILD flag, which is the case for this executable. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE050", "Version info StringFileInfo lacks SpecialBuild string" },
	{ "PE050_description", "Version information StringFileInfo block lacks the SpecialBuild string, which is required when "
		"version info has the VS_FF_SPECIALBUILD flag, which is the case for this executable. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE051", "Version info StringFileInfo has PrivateBuild string" },
	{ "PE051_description", "Version information StringFileInfo block has the PrivateBuild string, "
		"which should not be present when version info does not have the VS_FF_PRIVATEBUILD flag, "
		"which is the case for this executable. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE052", "Version info StringFileInfo has SpecialBuild string" },
	{ "PE052_description", "Version information StringFileInfo block has the SpecialBuild string, "
		"which should not be present when version info does not have the VS_FF_SPECIALBUILD flag, "
		"which is the case for this executable. "
		"Windows Explorer and other file version info viewers may not be able to show the version information correctly. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE053", "Version info fixed file version does not match StringFileInfo file version" },
	{ "PE053_description", "Version information StringFileInfo block FileVersion string does not match "
		"the fixed file version value. This is likely a configuration error. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'. "
		"StringFileInfo FileVersion string value: '{version_string}'; "
		"fixed file version value: '{fixed_version}'." },
	{ "PE054", "Version info fixed product version does not match StringFileInfo product version" },
	{ "PE054_description", "Version information StringFileInfo block ProductVersion string does not match "
		"the fixed product version value. This is likely a configuration error. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'. "
		"StringFileInfo ProductVersion string value: '{version_string}'; "
		"fixed product version value: '{fixed_version}'." },
	{ "PE066", "Version info StringFileInfo version string does not have standard format" },
	{ "PE066_description", "Version information StringFileInfo block version string does not have the standard format "
		"'MAJOR.MINOR.BUILD.REVISION', 'MAJOR.MINOR.BUILD' or 'MAJOR.MINOR'. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'. "
		"StringFileInfo version string value: '{version_string}'." },
	{ "PE067", "Version info StringFileInfo version string has suffix" },
	{ "PE067_description", "Version information StringFileInfo block version string has the standard format "
		"'MAJOR.MINOR.BUILD.REVISION', 'MAJOR.MINOR.BUILD' or 'MAJOR.MINOR', but also has a non-standard suffix in the end. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'. "
		"StringFileInfo version string value: '{version_string}'." },
	{ "PE068", "Version info string presence mismatch across StringFileInfo blocks" },
	{ "PE068_description", "Version information resource contains several StringFileInfo blocks. "
		"'{key}' string is present in some of them, but not the others. This is likely a configuration error. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE069", "Version info string mismatch across StringFileInfo blocks" },
	{ "PE069_description", "Version information resource contains several StringFileInfo blocks. "
		"'{key}' string value in some of them does not match the others. This is likely a configuration error. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'. "
		"'{key}' values which do not match: '{string1}', '{string2}'." },
	{ "PE100", "Incorrect version info file type" },
	{ "PE100_description", "Version information contains incorrect file type flag. "
		"Current file type: '{current_type}'. Suggested file type: '{expected_type}'. "
		"This type is suggested based on file extension ('{extension}'). "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE122", "Version info StringFileInfo is likely default" },
	{ "PE122_description", "Version information StringFileInfo block has a string which has a default value. "
		"This is likely because after adding a new version info block in the IDE, the string was not changed. "
		"Affected string key: '{key}', value: '{value}'. "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },
	{ "PE164", "Version info OriginalFileName does not match executable file name" },
	{ "PE164_description", "Version info 'OriginalFileName' ('{original_file_name}') string does not match the "
		"actual executable file name ('{entity_name}'). "
		"Affected StringFileInfo language: {string_lcid}, tag: '{string_lcid_tag}', location: '{string_lcid_location}', "
		"character set: '{string_cpid}', character set name: '{string_cpid_name}'." },

	{ "pe_cross_resource_version_info_rule", "PE cross-resource version info rule" },
	{ "PE039", "Multiple VERSIONINFO resources and multiple VERSIONINFO translations at the same time" },
	{ "PE039_description", "There are multiple VERSIONINFO resources with different languages in the executable. "
		"At the same time, there are multiple translations for a single VERSIONINFO resource. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE040", "Version info string mismatch across resources" },
	{ "PE040_description", "There are multiple VERSIONINFO resources with different languages in the executable. "
		"'{key}' string value in some of them does not match the others. This is likely a configuration error. "
		"Affected first resource language: {resource_lang1}, tag: '{resource_lang1_tag}', location: '{resource_lang1_location}'. "
		"Affected second resource language: {resource_lang2}, tag: '{resource_lang2_tag}', location: '{resource_lang2_location}'. "
		"'{key}' values which do not match: '{string1}', '{string2}'." },
	{ "PE028", "Version info fixed file info key mismatch across resources" },
	{ "PE028_description", "There are multiple VERSIONINFO resources with different languages in the executable. "
		"'{field_name}' fixed file info value in some of them does not match the others. This is likely a configuration error. "
		"Affected first resource language: {resource_lang1}, tag: '{resource_lang1_tag}', location: '{resource_lang1_location}'. "
		"Affected second resource language: {resource_lang2}, tag: '{resource_lang2_tag}', location: '{resource_lang2_location}'. "
		"'{field_name}' values which do not match: '{value1}', '{value2}'." },
	{ "PE029", "Version info string presence mismatch across resources" },
	{ "PE029_description", "There are multiple VERSIONINFO resources with different languages in the executable. "
		"'{key}' string value is present in some of them, but not the others. This is likely a configuration error. "
		"Affected first resource language: {resource_lang1}, tag: '{resource_lang1_tag}', location: '{resource_lang1_location}'. "
		"Affected second resource language: {resource_lang2}, tag: '{resource_lang2_tag}', location: '{resource_lang2_location}'." },

	{ "pe_version_info_format_rule", "PE version info format rule" },
	{ "PE030", "Invalid version info translation format" },
	{ "PE030_description", "Invalid VERSIONINFO resource VarFileInfo translation format. Each translation must be a pair of "
		"two 16-bit integers (language ID and character ID). There must be at least one pair, and all pairs must be unique. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE031", "Invalid version info resource format" },
	{ "PE031_description", "Invalid VERSIONINFO resource format. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Detailed error message: '{message}'. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE032", "Duplicate version info resource block" },
	{ "PE032_description", "VERSIONINFO resource '{block}' block is present more than once in the VERSIONINFO resource. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE033", "Duplicate version info translations" },
	{ "PE033_description", "Invalid VERSIONINFO resource VarFileInfo translation format. Each translation must be a pair of "
		"two 16-bit integers (language ID and character set ID). There must be at least one pair, and all pairs must be unique. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE041", "Duplicate version info string keys" },
	{ "PE041_description", "Invalid VERSIONINFO resource StringFileInfo format. Each string key must be unique. "
		"Some of the string keys in this VERSIONINFO resource are duplicate. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE070", "Duplicate version info string languages" },
	{ "PE070_description", "Invalid VERSIONINFO resource StringFileInfo format. Each string table language must be unique. "
		"Some of the string table languages in this VERSIONINFO resource are duplicate. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE071", "Invalid version info string languages" },
	{ "PE071_description", "Invalid VERSIONINFO resource StringFileInfo format. Each string table language must be unique. "
		"Each string table language must be a hexadecimal string consisting of the concatenation of the language and character set. "
		"Both language and character set IDs must be 4 characters long. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },
	{ "PE072", "Invalid version info block format" },
	{ "PE072_description", "Invalid VERSIONINFO resource block format. "
		"Analysis of this VERSIONINFO resource was skipped. "
		"Detailed error message: '{message}'. "
		"Affected version info block key: '{key}'. "
		"Affected resource language: {resource_lang}, tag: '{resource_lang_tag}', location: '{resource_lang_location}'." },

	{ "pe_main_icon_rule", "PE application icon rule" },
	{ "PE073", "Application icon is absent" },
	{ "PE073_description", "Application executable icon is absent. "
		"Windows Explorer and other file information viewers will not be able to show a custom icon." },
	{ "PE074", "Application icon height and width are different" },
	{ "PE074_description", "Application executable icon height and width are different: '{height}x{width}'. "
		"Windows Explorer and other file information viewers may not be able to show such icon. "
		"Icon resource name or id: '{icon}'." },
	{ "PE075", "Application icon group contains identical icons" },
	{ "PE075_description", "Application executable icon group contains icons of same size and bit count: "
		"'{height}x{width}', {bit_count} bits. "
		"Windows Explorer and other file information viewers may not be able to show such icon. "
		"Icon resource name or id: '{icon}'." },
	{ "PE076", "Application icon does not have all recommended sizes" },
	{ "PE076_description", "Application executable icon group does not provide all recommended icon sizes "
		"(16x16, 32x32, 48x48, 256x256). Missing size: '{height}x{width}'. "
		"This will make icon scaling imperfect in Windows Explorer and other file information viewers. "
		"Icon resource name or id: '{icon}'." },
	{ "PE077", "High resolution application icon have low bit count" },
	{ "PE077_description", "Application executable icon group provides a higher resolution icon ('{width}x{height}'), "
		"which lacks a higher bit count image (recommended: {bit_count} bits). "
		"This will make icon visual appearance worse in Windows Explorer and other file information viewers. "
		"Icon resource name or id: '{icon}'." },
	{ "PE078", "Application icon is multilingual" },
	{ "PE078_description", "Application executable icon is represented in several languages. "
		"This is a non-standard solution, which may make Windows Explorer and other file information viewers "
		"display the icon in an unexpected way. "
		"Analysis of icons in all languages except the first one was skipped. "
		"Icon resource name or id: '{icon}'." },
	{ "PE079", "Application icon format error" },
	{ "PE079_description", "Application executable icon format is not valid. "
		"Application icon analysis was skipped. Error details: {exception}. "
		"Icon resource name or id: '{icon}'." },

	{ "pe_cet_rule", "PE CET Shadow stack rule" },
	{ "PE080", "CET stack protection is not enabled" },
	{ "PE080_description", "CET-based (Intel Control Flow Enforcement Technology) stack protection is not enabled for the executable. "
		"Shadow stack is a hardware-enforced read-only memory region that helps keep record of the intended control-flow of the program. "
		"On supported hardware, call instructions push the return address on both stacks and return instructions compare the values "
		"and issues a CPU exception if there is a return address mismatch. "
		"Having this mitigation disabled may make exploitation of some vulnerabilities easier." },

	{ "pe_not_recommended_import_rule", "PE not recommended imports rule" },
	{ "PE081", "Potentially insecure WinAPI import" },
	{ "PE081_description", "The executable imports a Windows function which may be insecure if used incorrectly. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE082", "Deprecated WinAPI import" },
	{ "PE082_description", "The executable imports a Windows function which is deprecated. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE083", "Insecure WinAPI import" },
	{ "PE083_description", "The executable imports a Windows function which is considered insecure. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE084", "Internal WinAPI import" },
	{ "PE084_description", "The executable imports a Windows function which is internal to the "
		"operating system and is not recommended for use. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE085", "Potentially insecure WinAPI delay import" },
	{ "PE085_description", "The executable delay imports a Windows function which may be insecure if used incorrectly. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE086", "Deprecated WinAPI delay import" },
	{ "PE086_description", "The executable delay imports a Windows function which is deprecated. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE087", "Insecure WinAPI delay import" },
	{ "PE087_description", "The executable delay imports a Windows function which is considered insecure. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE088", "Internal WinAPI delay import" },
	{ "PE088_description", "The executable delay imports a Windows function which is internal to the "
		"operating system and is not recommended for use. "
		"Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE089", "XBox-only WinAPI import" },
	{ "PE089_description", "The executable imports an XBox-only function while not being targeted "
		"for XBox. Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },
	{ "PE090", "XBox-only WinAPI delay import" },
	{ "PE090_description", "The executable delay imports an XBox-only function while not being targeted "
		"for XBox. Library: '{dll}'; function name: '{api}'. Microsoft-suggested mitigation: '{mitigation}'." },

	{ "pe_ansi_import_rule", "PE ANSI imports rule" },
	{ "PE091", "ANSI WinAPI import" },
	{ "PE091_description", "The executable imports an ANSI WinAPI function version. "
		"Windows internally uses Unicode, which means that calling ANSI functions will always incur some "
		"encoding conversion cost. This may also cause localizaton compatibility issues. "
		"It is recommended to call the Unicode counterpart ('{unicode_api}') if available. "
		"Library: '{dll}'; function name: '{api}'." },
	{ "PE092", "ANSI WinAPI delay import" },
	{ "PE092_description", "The executable delay imports an ANSI WinAPI function version. "
		"Windows internally uses Unicode, which means that calling ANSI functions will always incur some "
		"encoding conversion cost. This may also cause localizaton compatibility issues. "
		"It is recommended to call the Unicode counterpart ('{unicode_api}') if available. "
		"Library: '{dll}'; function name: '{api}'." },

	{ "pe_directory_sections_rule", "PE directory sections rule" },
	{ "PE093", "Executable export directory" },
	{ "PE093_description", "The executable has an export directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE094", "Writable export directory" },
	{ "PE094_description", "The executable has an export directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE095", "Executable import directory" },
	{ "PE095_description", "The executable has an import directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE096", "Writable import directory" },
	{ "PE096_description", "The executable has an import directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE097", "Executable exception directory" },
	{ "PE097_description", "The executable has an exception directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE098", "Writable exception directory" },
	{ "PE098_description", "The executable has an exception directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE101", "Executable relocation directory" },
	{ "PE101_description", "The executable has a relocation directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE102", "Writable relocation directory" },
	{ "PE102_description", "The executable has a relocation directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE103", "Executable debug directory" },
	{ "PE103_description", "The executable has a debug directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE104", "Writable debug directory" },
	{ "PE104_description", "The executable has a debug directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE105", "Executable TLS directory" },
	{ "PE105_description", "The executable has a TLS (thread local storage) "
		"directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE106", "Writable TLS directory" },
	{ "PE106_description", "The executable has a TLS (thread local storage) "
		"directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE107", "Executable load configuration directory" },
	{ "PE107_description", "The executable has a load configuration directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE108", "Writable load configuration directory" },
	{ "PE108_description", "The executable has a load configuration directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE109", "Executable bound import directory" },
	{ "PE109_description", "The executable has a bound import directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE110", "Writable bound import directory" },
	{ "PE110_description", "The executable has a bound import directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE111", "Executable delay import directory" },
	{ "PE111_description", "The executable has a delay import directory which is placed to the executable section. "
		"This directory should be placed to read-only memory." },
	{ "PE112", "Writable delay import directory" },
	{ "PE112_description", "The executable has a delay import directory which is placed to the writable section. "
		"This directory should be placed to read-only memory." },
	{ "PE113", "Executable IAT directory" },
	{ "PE113_description", "The executable has an IAT (import address table) "
		"directory which is placed to the executable section. "
		"This directory should be placed to read-only or read-write memory." },
	{ "PE114", "Writable entry point" },
	{ "PE114_description", "The executable has an entry point which is placed to the writable section. "
		"Entry point should be located in readable and executable memory." },

	{ "pe_checksum_rule", "PE checksum rule" },
	{ "PE115", "Absent checksum" },
	{ "PE115_description", "The executable does not have a checksum. "
		"Checksum should be calculated for drivers and signed images." },
	{ "PE116", "Incorrect checksum" },
	{ "PE116_description", "The executable checksum does not match its expected checksum value. "
		"Image checksum set in its headers: '{checksum:#08x}'. Expected checksum: '{expected_checksum:#08x}'." },

	{ "pe_rich_data_rule", "PE Rich data rule" },
	{ "PE119", "Potentially preview toolchain version used" },
	{ "PE119_description", "The executable (or some of its object files) was built using "
		"a potentially preview version of a Microsoft toolchain. "
		"The code may be not optimal, have bugs, or be insecure. "
		"Tool type: '{type}', product version: '{version}'."},
	{ "PE120", "Preview toolchain version used" },
	{ "PE120_description", "The executable (or some of its object files) was built using "
		"a preview version of a Microsoft toolchain. "
		"The code may be not optimal, have bugs, or be insecure. "
		"Tool type: '{type}', product version: '{version}'."},
	{ "PE121", "Too old toolchain version used" },
	{ "PE121_description", "The executable (or some of its object files) was built using "
		"a too old version of a Microsoft toolchain. "
		"The code may be not optimal, have bugs, or be insecure. "
		"Tool type: '{type}', product version: '{version}'."},

	{ "pe_application_manifest_rule", "PE Manifest rule" },
	{ "PE128", "Manifest does not exist" },
	{ "PE128_description", "The executable lacks the manifest file or the manifest resource. "
		"Several system mitigations and optimizations are disabled." },
	{ "PE129", "Manifest exists, but the executable is marked non-isolated" },
	{ "PE129_description", "The executable has the manifest file or manifest resource. "
		"However, the executable header specifies the non-isolated flag, which means "
		"that the manifest will not be loaded and used by the system. "
		"This is likely a configuration error. "
		"Several system mitigations and optimizations are disabled." },
	{ "PE130", "Both embedded and external manifests exist" },
	{ "PE130_description", "The executable has both the embedded manifest (in the resource "
		"directory) and external manifest (as a separate file). "
		"This may lead to issues when running on different Windows versions. "
		"On Windows Vista and later, the embedded manifest always precedes the external one." },
	{ "PE131", "UAC virtualization is enabled" },
	{ "PE131_description", "The executable does not specify UAC virtualization options in the manifest. "
		"The system will virtualize access to some file system "
		"directories and registry paths, which may be unintended. "
		"UAC virtualization should be disabled for newer applications." },
	{ "PE132", "Latest Windows version is not supported" },
	{ "PE132_description", "The executable does not specify the support of the latest "
		"Windows versions (Windows 10 and 11, Windows Server 2016, 2019 and 2022) in the manifest. "
		"Windows may apply OS-specific shims and mitigations based on this, "
		"which may be unintended. "
		"Newer applications should declare support for all operating systems." },
	{ "PE133", "Manifest has gaps in supported OS list" },
	{ "PE133_description", "The executable has the operating systems support section "
		"in its manifest. However, some operating system versions are omitted. "
		"Newer applications should list support for all operating systems "
		"without gaps." },
	{ "PE134", "Segment Heap is not used" },
	{ "PE134_description", "The executable does not specify the heap type in the manifest. "
		"For newer applications, it is recommended to use Segment Heap, "
		"which is a modern heap implementation that will generally reduce overall memory usage." },
	{ "PE135", "Executable is not long path aware" },
	{ "PE135_description", "The executable does not specify long path awareness in the manifest. "
		"Newer applications should be marked as long path aware." },
	{ "PE136", "Unknown heap type is used" },
	{ "PE136_description", "The executable specifies the unknown heap type in the manifest ('{type}'). " },
	{ "PE137", "Supported OS list is absent" },
	{ "PE137_description", "The executable does not specify the supported operating systems list "
		"in the manifest. Windows may apply OS-specific shims and mitigations based on this, "
		"which may be unintended. "
		"Newer applications should declare support for all operating systems." },
	{ "PE138", "Printer driver isolation is not specified" },
	{ "PE138_description", "The executable does not specify the printer driver isolation option "
		"in the manifest. Printer driver isolation improves application reliability by enabling "
		"printer drivers to run in processes that are separate from the process "
		"in which the print spooler runs. That is, the application will not crash "
		"if the printer driver has an error." },
	{ "PE139", "Auto-elevate option is specified" },
	{ "PE139_description", "The executable specifies the auto-elevate option "
		"in the manifest. This option is intended for the internal use." },
	{ "PE142", "Executable is not DPI aware" },
	{ "PE142_description", "The executable does not specify DPI awareness in the manifest. "
		"Newer Window-based applications should be PerMonitorV2 DPI aware." },
	{ "PE143", "Executable is not DPI aware, but scales GDI" },
	{ "PE143_description", "The executable does not specify DPI awareness in the manifest. "
		"At the same time, executable requests GDI Window elements scaling from the operating system. "
		"Newer Window-based applications should be PerMonitorV2 DPI aware." },
	{ "PE144", "Executable is not PerMonitorV2 DPI aware" },
	{ "PE144_description", "The executable specifies DPI awareness in the manifest. "
		"At the same time, the executable is not PerMonitorV2 DPI aware. "
		"Newer Windows-based applications should be PerMonitorV2 DPI aware." },
	{ "PE160", "Executable with UI access is not signed" },
	{ "PE160_description", "The executable specifies uiAccess='true' in the manifest. "
		"At the same time, executable is not signed. "
		"'uiAccess' property will be ignored by the operating system." },

	{ "pe_image_errors_rule", "PE Image errors rule" },
	{ "PE140", "Unable to load image" },
	{ "PE140_description", "The image was not loaded due to the format error. "
		"Lower-level error details: '{exception}'."},
	{ "PE141", "Image loaded with warnings" },
	{ "PE141_description", "The image was loaded with some warnings. "
		"Lower-level warning details: '{exception}'." },

	{ "pe_manifest_format_rule", "PE Manifest format rule" },
	{ "PE145", "Manifest load error" },
	{ "PE145_description", "Manifest was not loaded due to XML format error. "
		"Lower-level error details: '{exception}'." },
	{ "PE146", "Manifest format error" },
	{ "PE146_description", "Manifest was loaded, but has some format issues. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE149", "Unsupported application manifest element" },
	{ "PE149_description", "Executable has an application manifest, which has "
		"an assembly-specific element. This element does not make sense in application manifests. "
		"Element name: '{tag}'." },
	{ "PE150", "Unsupported assembly manifest element" },
	{ "PE150_description", "Executable has an assembly manifest, which has "
		"an application-specific element. This element does not make sense in assembly manifests. "
		"Element name: '{tag}'." },
	
	{ "pe_load_config_flags_rule", "PE load configuration flags rule" },
	{ "PE147", "Process heap is always executable" },
	{ "PE147_description", "Image load configuration directory has "
		"the HEAP_CREATE_ENABLE_EXECUTE heap flag set for the executable. "
		"Process default heap will be always executable even if DEP (data execution prevention) is enabled. "
		"This will allow self-modifying code (including exploits) to be executed easily "
		"without the need to manipulate heap memory attributes." },
	{ "PE148", "Global flags overridden" },
	{ "PE148_description", "Image load configuration directory has "
		"non-zero values for the global flags set or clear fields. "
		"Global flags are system-specific and are intended for low-level debugging purposes. "
		"They should not be overridden on production executables." },

	{ "pe_dotnet_header_format_rule", "PE .NET header format rule" },
	{ "PE151", ".NET header format error" },
	{ "PE151_description", ".NET header was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },

	{ "pe_debug_directory_format_rule", "PE Debug directory format rule" },
	{ "PE152", "Debug directory format error" },
	{ "PE152_description", "Debug directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },

	{ "pe_import_format_rule", "PE Import format rule" },
	{ "PE153", "Import directory format error" },
	{ "PE153_description", "Import directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE154", "Delay import directory format error" },
	{ "PE154_description", "Delay import directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },

	{ "pe_export_directory_format_rule", "PE Export directory format rule" },
	{ "PE155", "Export directory format error" },
	{ "PE155_description", "Export directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },

	{ "pe_load_config_directory_format_rule", "PE Load Configuration directory format rule" },
	{ "PE156", "Load configuration directory format error" },
	{ "PE156_description", "Load configuration directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE157", "Load configuration structure size does not match specific version" },
	{ "PE157_description", "Load configuration structure size does not match any "
		"specific known version of that structure. "
		"The loader may not be able to correctly load all fields of the structure." },

	{ "pe_resource_directory_format_rule", "PE Resource directory format rule" },
	{ "PE158", "Resource directory format error" },
	{ "PE158_description", "Resource directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE159", "Resource directory has loops" },
	{ "PE159_description", "Resource directory has loops. "
		"This can prevent Windows from correctly parsing the resource directory." },

	{ "pe_vc_feature_rule", "PE VC Feature debug directory rule" },
	{ "PE161", "/sdl switch is not always used" },
	{ "PE161_description", "The executable was built using a modern VC++ compiler, "
		"but not all object files were built with the '/sdl' compiler switch enabled. "
		"Consider enabling that switch for all source files and libraries to enable "
		"additional compiler Security Development Lifecycle checks. "
		"Count of objects with '/sdl' disabled: {count}." },
	{ "PE162", "Pre-C++11 object files are used" },
	{ "PE162_description", "The executable was built using a modern VC++ compiler, "
		"but some object files were built using an older toolset with the C++ version "
		"earlier than C++11. Consider migrating all code to the new compiler to improve "
		"its efficiency and safety. "
		"Pre-C++11 object count: {count}." },
			
	{ "pe_manifest_assembly_version_rule", "PE Manifest assembly version rule" },
	{ "PE163", "Manifest assembly version does not match file/product version" },
	{ "PE163_description", "The executable has a manifest with assembly version '{assembly_version}'. "
		"This version does not match any version from the version info: "
		"neither file version ('{file_version}') nor product version ('{product_version}'). "
		"This may be a configuration error." },

	{ "pe_combined_version_info_rule", "PE Combined version info rule" },
	{ "PE044", "Fixed product version info mismatch across executables" },
	{ "PE044_description", "The most popular fixed product version across project executables "
		"is '{most_popular_version}'. The fixed product version of the executable does not match it: "
		"'{version}'." },
	{ "PE045", "Version info translations mismatch across executables" },
	{ "PE045_description", "The most popular version info translations across project executables "
		"are '{most_popular_translations}'. The version info translations of the executable do not match it: "
		"'{translations}'." },
	{ "PE046", "Version info string mismatch across executables" },
	{ "PE046_description", "The most popular version info '{string_id}' string across project executables "
		"is '{most_popular_string}'. The version info string of the executable does not match it: "
		"'{string}'. This string should likely be the same for all executables of the project." },

	{ "pe_security_directory_format_rule", "PE Security directory format rule" },
	{ "PE047", "Security directory format error" },
	{ "PE047_description", "Security directory was loaded with errors. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },

	{ "pe_authenticode_rule", "PE Authenticode signature rule" },
	{ "PE017", "Executable is not signed" },
	{ "PE017_description", "Executable is not signed. "
		"This enables the attacker to do any modifications to the disk image of the executable without being detected." },
	{ "PE021", "Signature check is not enforced" },
	{ "PE021_description", "Signature check is not enforced. "
		"The executable is signed, but the signature check is not enforced. "
		"The loader will not prevent the image from being loaded and executed even if the signature is not correct. "
		"This effectively enables the attacker to do any modifications to the disk image of the executable without being detected." },
	{ "PE048", "Authenticode signature format error" },
	{ "PE048_description", "Authenticode signature format error for {signature_info}. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE165", "Incorrect Authenticode image hash value" },
	{ "PE165_description", "Authenticode image hash value is not correct for {signature_info}. "
		"This makes the Authenticode signature invalid." },
	{ "PE166", "Authenticode certificate store format warning" },
	{ "PE166_description", "Authenticode certificate store format warning for {signature_info}. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE167", "Incorrect Authenticode image page hashes" },
	{ "PE167_description", "Authenticode image page hashes value is not correct for {signature_info}. "
		"This makes the Authenticode signature invalid." },
	{ "PE168", "Absent Authenticode image page hashes" },
	{ "PE168_description", "Authenticode image page hashes are absent for {signature_info}. "
		"Image page hashes do not impact security, although they are recommended for performance "
		"reasons to make image loading faster in some scenarios (especially when used with the "
		"/INTEGRITYCHECK linker option)."},
	{ "PE169", "Authenticode image page hashes check error" },
	{ "PE169_description", "Unable to check Authenticode image page hashes for {signature_info}. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE170", "Incorrect Authenticode message digest" },
	{ "PE170_description", "Authenticode message digest value is not correct for {signature_info}. "
		"This makes the Authenticode signature invalid." },
	{ "PE171", "Weak Authenticode image hash algorithm" },
	{ "PE171_description", "Authenticode image hash algorithm is too weak for {signature_info}. "
		"This may allow an attacker to modify the executable while keeping the signature valid. "
		"Hash algorithm used is '{hash_algorithm}'." },
	{ "PE172", "Unable to check Authenticode image signature" },
	{ "PE172_description", "Unable to check Authenticode image signature for {signature_info}. "
		"Error description: '{description}'. "
		"Lower-level error details: '{exception}'." },
	{ "PE173", "Weak Authenticode signature RSA key size" },
	{ "PE173_description", "Authenticode signature RSA key size is too small for {signature_info}. "
		"This may allow an attacker to modify the executable while keeping the signature valid. "
		"RSA key size is '{key_size}', while the recommended key size is at least '{min_key_size}'." },
	{ "PE174", "Weak Authenticode signature ECDSA curve" },
	{ "PE174_description", "Authenticode signature ECDSA curve is too weak for {signature_info}. "
		"This may allow an attacker to modify the executable while keeping the signature valid. "
		"ECDSA curve is '{curve}', while the recommended curve key size is at least '{min_key_size}'." },
	{ "PE175", "Incorrect Authenticode image signature" },
	{ "PE175_description", "Authenticode image signature is not correct for {signature_info}." },
	{ "PE176", "Absent Authenticode timestamp counter-signature" },
	{ "PE176_description", "Authenticode timestamp counter-signature is absent for {signature_info}. "
		"The signature will become invalid if the signing certificate is revoked or if it expires. "
		"Consider signing the executable with the timestamp counter-signature to avoid that." },
	{ "PE177", "Incorrect Authenticode timestamp counter-signature digest" },
	{ "PE177_description", "Authenticode timestamp counter-signature digest is not correct for {signature_info}. "
		"This makes the timestamp Authenticode signature invalid." },
	{ "PE178", "Weak Authenticode timestamp counter-signature digest algorithm" },
	{ "PE178_description", "Authenticode timestamp counter-signature "
		"digest algorithm is too weak for {signature_info}. "
		"This may allow an attacker to modify the executable while keeping the signature valid. "
		"Hash algorithm used is '{hash_algorithm}'." },
	{ "PE179", "Weak Authenticode timestamp counter-signature imprint digest algorithm" },
	{ "PE179_description", "Authenticode timestamp counter-signature imprint "
		"digest algorithm is too weak for {signature_info}. "
		"This may allow an attacker to modify the executable while keeping the signature valid. "
		"Hash algorithm used is '{hash_algorithm}'." },
	{ "PE180", "Incorrect Authenticode timestamp counter-signature" },
	{ "PE180_description", "Authenticode timestamp counter-signature is not correct for {signature_info}." },
	{ "PE181", "Absent Authenticode timestamp counter-signature signing time" },
	{ "PE181_description", "Authenticode timestamp counter-signature lacks signing time for {signature_info}. "
		"A valid timestamp counter-signature must have the correct signing time attribute."},
	{ "PE182", "Authenticode signature check error" },
	{ "PE182_description", "Unable to check Authenticode signature. "
		"Error details: {exception}" },
	{ "PE184", "Authenticode test signature" },
	{ "PE184_description", "Authenticode signature marked as test for {signature_info}. "
		"Test signatures should not be present in production executables." },
	{ "PE185", "Authenticode signing certificate empty subject DN" },
	{ "PE185_description", "Authenticode signing certificate subject distinguished name "
		"is empty for {signature_info}. "
		"Valid subject DN must contain at least one attribute." },
	{ "PE186", "Authenticode signing certificate subject DN missing attributes" },
	{ "PE186_description", "Authenticode signing certificate subject distinguished name "
		"is missing some of the most common attributes for {signature_info}. "
		"Missing attributes are: '{attributes}'" },
	{ "PE187", "Authenticode signing certificate subject DN invalid attributes" },
	{ "PE187_description", "Authenticode signing certificate subject distinguished name "
		"has some attributes which are not valid for {signature_info}. "
		"Attributes which have invalid ASN.1 DER repersentations: '{attributes}'" },

	{ "pe_combined_dll_import_names_case_rule", "PE Combined imported DLL name case rule" },
	{ "PE183", "Imported DLL name and the actual DLL file name have different case" },
	{ "PE183_description", "The executable imports the '{imported_library}' library. "
		"The actual library file name is '{library_file_name}'. Importing a library with different letter "
		"case will cause issues if the application is installed in a Windows folder which is set up to be "
		"case-sensitive."},
	{ "PE188", "Delay-imported DLL name and the actual DLL file name have different case" },
	{ "PE188_description", "The executable delay-imports the '{imported_library}' library. "
		"The actual library file name is '{library_file_name}'. Importing a library with different letter "
		"case will cause issues if the application is installed in a Windows folder which is set up to be "
		"case-sensitive." },

	{ "pe_combined_cross_signature_check_rule", "PE Combined digital signature cross-check rule" },
	{ "PE189", "Signed executable imports unsigned DLL" },
	{ "PE189_description", "The digitally signed executable imports the unsigned '{imported_library}' library. "
		"This is a security risk which significantly diminishes the value of the executable digital signature." },
	{ "PE190", "Signed executable delay-imports unsigned DLL" },
	{ "PE190_description", "The digitally signed executable delay-imports the unsigned '{imported_library}' library. "
		"This is a security risk which significantly diminishes the value of the executable digital signature." },

	{ "pe_authenticode_signature_info_root", "root signature" },
	{ "pe_authenticode_signature_info_nested", "nested signature (index {index})" },
	{ "pe_authenticode_timestamp_signature_info_root", "timestamp root signature" },
	{ "pe_authenticode_timestamp_signature_info_nested", "timestamp nested signature (index {index})" },

	{ "pe_manifest_invalid_manifest_version",
		"Invalid manifest version string. The only supported manifest version is 1.0." },
	{ "pe_manifest_empty_manifest", "Manifest is an empty XML." },
	{ "pe_manifest_absent_manifest_version",
		"Manifest version is empty. The only supported manifest version is 1.0." },
	{ "pe_manifest_no_inherit_element_not_first",
		"<noInherit> element does not come first. "
		"It should be the first subelement of the <assembly> element." },
	{ "pe_manifest_absent_assembly_identity", "Required <assemblyIdentity> element is absent." },
	{ "pe_manifest_invalid_assembly_identity_element_position",
		"<assemblyIdentity> element does not have a valid position. It should come either as "
		"a first subelement of the <assembly> element, or after the <noInherit> element." },
	{ "pe_manifest_multiple_assembly_identities",
		"Multiple <assemblyIdentity> elements are present." },
	{ "pe_manifest_multiple_no_inherit_elements",
		"Multiple <noInherit> elements are present." },
	{ "pe_manifest_absent_assembly_identity_type",
		"Required <assemblyIdentity> 'type' attribute is absent." },
	{ "pe_manifest_absent_assembly_identity_name",
		"Required <assemblyIdentity> 'name' attribute is absent." },
	{ "pe_manifest_absent_assembly_identity_version",
		"Required <assemblyIdentity> 'version' attribute is absent." },
	{ "pe_manifest_multiple_compatibility_elements",
		"Multiple <compatibility> elements are present." },
	{ "pe_manifest_multiple_maxversiontested_elements",
		"Multiple <maxversiontested> elements are present." },
	{ "pe_manifest_invalid_dependencies", "Invalid <dependentAssembly> element contents. "
		"This element should contain a single <dependency> element, which in turn "
		"should contain one or more <dependentAssembly> elements." },
	{ "pe_manifest_invalid_dependend_assembly_element_position",
		"The first subelement of <dependentAssembly> must be <assemblyIdentity>, "
		"which is not the case." },
	{ "pe_manifest_absent_file_name",
		"Required <file> 'name' attribute is absent." },
	{ "pe_manifest_absent_clsid",
		"Required <comClass> 'clsid' attribute is absent." },
	{ "pe_manifest_absent_tlbid",
		"Required <typelib> 'tlbid' attribute is absent." },
	{ "pe_manifest_absent_version",
		"Required <typelib> 'version' attribute is absent." },
	{ "pe_manifest_absent_helpdir",
		"Required <typelib> 'helpdir' attribute is absent." },
	{ "pe_manifest_absent_iid",
		"Required <comInterfaceExternalProxyStub> or <comInterfaceProxyStub> "
		"'iid' attribute is absent." },
	{ "pe_manifest_multiple_application_elements",
		"Multiple <application> elements are present." },
	{ "pe_manifest_multiple_windows_settings_elements",
		"Multiple <windowsSettings> elements are present inside the <application> element." },
	{ "pe_manifest_multiple_active_code_page_elements",
		"Multiple <activeCodePage> elements are present inside the <windowsSettings> element." },
	{ "pe_manifest_multiple_auto_elevate_elem",
		"Multiple <autoElevate> elements are present." },
	{ "pe_manifest_invalid_auto_elevate_elem",
		"Invalid <autoElevate> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_disable_theming_elem",
		"Multiple <disableTheming> elements are present." },
	{ "pe_manifest_invalid_disable_theming_elem",
		"Invalid <disableTheming> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_disable_window_filtering_elem",
		"Multiple <disableWindowFiltering> elements are present." },
	{ "pe_manifest_invalid_disable_window_filtering_elem",
		"Invalid <disableWindowFiltering> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_gdi_scaling_elem",
		"Multiple <gdiScaling> elements are present." },
	{ "pe_manifest_invalid_gdi_scaling_elem",
		"Invalid <gdiScaling> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_high_resolution_scrolling_aware_elem",
		"Multiple <highResolutionScrollingAware> elements are present." },
	{ "pe_manifest_invalid_high_resolution_scrolling_aware_elem",
		"Invalid <highResolutionScrollingAware> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_ultra_high_resolution_scrolling_aware_elem",
		"Multiple <ultraHighResolutionScrollingAware> elements are present." },
	{ "pe_manifest_invalid_ultra_high_resolution_scrolling_aware_elem",
		"Invalid <ultraHighResolutionScrollingAware> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_long_path_aware_elem",
		"Multiple <longPathAware> elements are present." },
	{ "pe_manifest_invalid_long_path_aware_elem",
		"Invalid <longPathAware> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_printer_driver_isolation_elem",
		"Multiple <printerDriverIsolation> elements are present." },
	{ "pe_manifest_invalid_printer_driver_isolation_elem",
		"Invalid <printerDriverIsolation> element value. It should be 'true' or 'false'." },
	{ "pe_manifest_multiple_dpi_aware_elements",
		"Multiple <dpiAware> elements are present." },
	{ "pe_manifest_multiple_dpi_awareness_elements",
		"Multiple <dpiAwareness> elements are present." },
	{ "pe_manifest_multiple_heap_type_elements",
		"Multiple <heapType> elements are present." },
	{ "pe_manifest_multiple_trust_info_elements",
		"Multiple <trustInfo> elements are present." },
	{ "pe_manifest_multiple_security_elements",
		"Multiple <security> elements are present inside the <trustInfo> element." },
	{ "pe_manifest_multiple_requested_privileges_elements",
		"Multiple <requestedPrivileges> elements are present inside the <security> element." },
	{ "pe_manifest_multiple_requested_execution_level_elements",
		"Multiple <requestedExecutionLevel> elements are present inside "
		"the <requestedPrivileges> element." },
	{ "pe_manifest_absent_requested_execution_level",
		"Required 'level' attribute is absent in the <requestedExecutionLevel> element." },
	{ "pe_manifest_absent_requested_execution_ui_access",
		"Required 'uiAccess' attribute is absent in the <requestedExecutionLevel> element." },
	{ "pe_manifest_absent_msix_publisher",
		"Required <msix> 'publisher' attribute is absent." },
	{ "pe_manifest_absent_msix_package_name",
		"Required <msix> 'packageName' attribute is absent." },
	{ "pe_manifest_absent_msix_application_id",
		"Required <msix> 'applicationId' attribute is absent." },
	{ "pe_manifest_multiple_msix_elements",
		"Multiple <msix> elements are present." },
	{ "pe_manifest_empty_progids",
		"No <progid> elements are present inside the <comClass> element." },
	{ "pe_manifest_empty_window_class",
		"<windowClass> element has an empty value." },
	{ "pe_manifest_absent_declaration", "Absent manifest XML declaration." },

	{ "pe_dotnet_unable_to_deserialize_header", "Unable to deserialize .NET header"},
	{ "pe_dotnet_unable_to_deserialize_footer", "Unable to deserialize .NET header footer"},
	{ "pe_dotnet_version_length_not_aligned",
		".NET header version length is not aligned in memory"},
	{ "pe_unable_to_deserialize_runtime_version",
		"Unable to deserialize .NET runtime version"},
	{ "pe_dotnet_too_long_runtime_version_string", "Too long .NET runtime version string"},
	{ "pe_dotnet_unable_to_deserialize_stream_header",
		"Unable to deserialize .NET stream header"},
	{ "pe_dotnet_unable_to_deserialize_stream_data",
		"Unable to deserialize .NET stream data"},
	{ "pe_dotnet_duplicate_stream_names", ".NET header has duplicate stream names"},

	{ "pe_debug_no_rva_and_file_offset", "Debug directory has neither RVA nor file offset." },
	{ "pe_debug_invalid_file_offset", "Invalid debug directory file offset value." },
	{ "pe_debug_invalid_debug_directory_size", "Invalid debug directory size." },
	{ "pe_debug_excessive_data_in_directory", "Excessive data in debug directory." },
	{ "pe_debug_unable_to_load_entries", "Unable to load debug directory entries." },
	{ "pe_debug_unable_to_load_raw_data", "Unable to load raw debug directory data." },
	{ "pe_debug_rva_and_file_offset_do_not_match",
		"Debug directory RVA and file offset do not match." },
	{ "pe_debug_too_many_debug_directories", "Too many debug directories." },
	{ "pe_debug_too_big_raw_data", "Too large debug directory raw data." },

	{ "pe_import_invalid_library_name", "Unable to read imported library name." },
	{ "pe_import_invalid_import_hint", "Unable to read import hint." },
	{ "pe_import_invalid_import_name", "Unable to read import name." },
	{ "pe_import_invalid_hint_name_rva", "Invalid import hint/name RVA." },
	{ "pe_import_lookup_and_address_table_thunks_differ",
		"Import table lookup and address table thunks differ." },
	{ "pe_import_invalid_import_directory",
		"Unable to read imported library structure." },
	{ "pe_import_zero_iat_and_ilt",
		"Imported library has zero import address table and import lookup table RVAs." },
	{ "pe_import_invalid_import_ordinal", "Invalid import ordinal value." },
	{ "pe_import_zero_iat",
		"Imported library has zero import address table RVA." },
	{ "pe_import_invalid_imported_library_iat_ilt",
		"Unable to read imported library import address or import lookup table." },
	{ "pe_import_empty_library_name", "Empty imported library name." },
	{ "pe_import_empty_import_name", "Empty imported function name." },
	{ "pe_import_address_and_unload_table_thunks_differ",
		"Delay load import address and unload table thunk RVAs differ." },

	{ "pe_exports_invalid_library_name", "Unable to read export library name." },
	{ "pe_exports_invalid_forwarded_name",
		"Unable to read exported forwarded library name." },
	{ "pe_exports_invalid_name_list", "Unable to read exported name list." },
	{ "pe_exports_invalid_address_list", "Unable to read exported address list." },
	{ "pe_exports_invalid_name_ordinal", "Unable to read exported name ordinal." },
	{ "pe_exports_invalid_name_rva", "Invalid exported name RVA." },
	{ "pe_exports_empty_name", "Empty exported symbol name." },
	{ "pe_exports_unsorted_names",
		"Exported symbol names are not sorted alphabetically." },
	{ "pe_exports_invalid_rva", "Invalid exported symbol RVA." },
	{ "pe_exports_invalid_directory", "Unable to read exported directory." },
	{ "pe_exports_invalid_address_list_number_of_functions",
		"Too many exported symbols." },
	{ "pe_exports_invalid_address_list_number_of_names",
		"Too many exported names." },

	{ "pe_load_config_invalid_dynamic_relocation_table_section_index",
		"Invalid dynamic relocation table section index." },
	{ "pe_load_config_invalid_dynamic_relocation_table_section_offset",
		"Invalid dynamic relocation table section offset." },
	{ "pe_load_config_unknown_dynamic_relocation_table_version",
		"Unknown dynamic relocation table version. Only V1 and V2 are supported." },
	{ "pe_load_config_invalid_lock_prefix_table",
		"Unable to load lock prefix table." },
	{ "pe_load_config_invalid_safeseh_handler_table",
		"Unable to load SafeSEH handler table." },
	{ "pe_load_config_invalid_cf_function_table",
		"Unable to load CF (control flow) function table." },
	{ "pe_load_config_invalid_cf_export_suppression_table",
		"Unable to load CF (control flow) guard export suppression table." },
	{ "pe_load_config_invalid_cf_longjump_table",
		"Unable to load CF (control flow) guard longjump table." },
	{ "pe_load_config_unknown_chpe_metadata_type",
		"Unknown hybrid PE metadata type.", },
	{ "pe_load_config_invalid_chpe_metadata",
		"Unable to load hybrid PE metadata." },
	{ "pe_load_config_invalid_chpe_range_entries",
		"Unable to load hybrid PE range entries." },
	{ "pe_load_config_invalid_dynamic_relocation_table",
		"Unable to load dynamic relocation table." },
	{ "pe_load_config_invalid_dynamic_relocation_table_v2_size",
		"Invalid dynamic relocation table V2 size." },
	{ "pe_load_config_invalid_dynamic_relocation_table_v1_size",
		"Invalid dynamic relocation table V1 size." },
	{ "pe_load_config_invalid_dynamic_relocation_header_size",
		"Invalid dynamic relocation header size." },
	{ "pe_load_config_invalid_dynamic_relocation_fixup_info_size",
		"Invalid dynamic relocation fixup info size." },
	{ "pe_load_config_invalid_dynamic_relocation_prologue",
		"Unable to load dynamic relocation prologue." },
	{ "pe_load_config_invalid_dynamic_relocation_epilogue",
		"Unable to load dynamic relocation epilogue." },
	{ "pe_load_config_unknown_dynamic_relocation_symbol",
		"Unknown dynamic relocation symbol." },
	{ "pe_load_config_invalid_dynamic_relocation_block_size",
		"Invalid dynamic relocation block size." },
	{ "pe_load_config_unaligned_dynamic_relocation_block",
		"Unaligned dynamic relocation block." },
	{ "pe_load_config_unknown_arm64x_relocation_type",
		"Unknown ARM64X relocation type." },
	{ "pe_load_config_invalid_arm64x_dynamic_relocation_copy_data_size",
		"Invalid ARM64X dynamic relocation (copy data) size." },
	{ "pe_load_config_invalid_arm64x_dynamic_relocation_add_delta_size",
		"Invalid ARM64X dynamic relocation (add delta) size." },
	{ "pe_load_config_invalid_base_relocation_size",
		"Invalid base relocation size." },
	{ "pe_load_config_invalid_dynamic_relocation_size",
		"Invalid dynamic relocation size." },
	{ "pe_load_config_invalid_cf_guard_table_size",
		"Invalid CF (control flow) guard table size." },
	{ "pe_load_config_unsorted_cf_guard_table",
		"Unsorted CF (control flow) guard table." },
	{ "pe_load_config_invalid_dynamic_relocation_epilogue_size",
		"Invalid dynamic relocation epilogue size." },
	{ "pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptor_size",
		"Invalid dynamic relocation epilogue branch descriptor size." },
	{ "pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptors",
		"Invalid dynamic relocation epilogue branch descriptors." },
	{ "pe_load_config_invalid_dynamic_relocation_epilogue_branch_descriptor_bit_map",
		"Invalid dynamic relocation epilogue branch descriptor bit map." },
	{ "pe_load_config_invalid_enclave_config",
		"Unable to load enclave configuration." },
	{ "pe_load_config_invalid_enclave_config_extra_data",
		"Unable to load enclave configuration extra data." },
	{ "pe_load_config_invalid_enclave_import_extra_data",
		"Unable to load enclave import extra data." },
	{ "pe_load_config_invalid_enclave_import_name",
		"Unable to load enclave import name." },
	{ "pe_load_config_invalid_volatile_metadata",
		"Unable to load volatile metadata." },
	{ "pe_load_config_unaligned_volatile_metadata_access_rva_table_size",
		"Unaligned volatile metadata access table size." },
	{ "pe_load_config_unaligned_volatile_metadata_range_table_size",
		"Unaligned volatile metadata range table size." },
	{ "pe_load_config_invalid_volatile_metadata_access_rva_table",
		"Unable to load volatile metadata access RVA table." },
	{ "pe_load_config_invalid_volatile_metadata_range_table",
		"Unable to load volatile metadata range table." },
	{ "pe_load_config_invalid_ehcont_targets",
		"Unable to load exception handling continuation targets." },
	{ "pe_load_config_unsorted_ehcont_targets",
		"Unsorted exception handling continuation target RVAs." },
	{ "pe_load_config_invalid_xfg_type_based_hash_rva",
		"Invalid XFG type-based hash RVA." },
	{ "pe_load_config_invalid_func_override_size",
		"Invalid func override block size." },
	{ "pe_load_config_invalid_func_override_rvas_size",
		"Invalid func override RVAs size." },
	{ "pe_load_config_invalid_func_override_fixup",
		"Unable to load func override fixup entry." },
	{ "pe_load_config_invalid_func_override_dynamic_relocation",
		"Unable to load func override dynamic relocation." },
	{ "pe_load_config_invalid_func_override_rvas",
		"Unable to load func override RVAs." },
	{ "pe_load_config_invalid_bdd_info_size",
		"Invalid func override BDD info size." },
	{ "pe_load_config_invalid_bdd_info_entry",
		"Unable to load func override BDD info entry." },
	{ "pe_load_config_invalid_bdd_dynamic_relocations",
		"Unable to load func override BDD info dynamic relocations." },
	{ "pe_load_config_unknown_bdd_info_entry_version",
		"Unknown BDD info entry version." },
	{ "pe_load_config_invalid_load_config_directory",
		"Unable to load load configuration directory." },
	{ "pe_load_config_invalid_guard_memcpy_function_pointer_va",
		"Invalid guard memcpy function pointer virtual address." },
	{ "pe_load_config_invalid_cast_guard_os_determined_failure_mode_va",
		"Invalid CastGuard OS determined failure mode virtual address." },
	{ "pe_load_config_invalid_guard_xfg_check_function_pointer_va",
		"Invalid extended guard check function pointer virtual address." },
	{ "pe_load_config_invalid_guard_xfg_dispatch_function_pointer_va",
		"Invalid extended guard dispatch function pointer virtual address." },
	{ "pe_load_config_invalid_guard_xfg_table_dispatch_function_pointer_va",
		"Invalid extended guard table dispatch function pointer virtual address." },
	{ "pe_load_config_invalid_enclave_import_array",
		"Unable to load enclave import array." },
	{ "pe_load_config_invalid_dynamic_relocation_entry",
		"Unable to load dynamic relocation entry." },
	{ "pe_load_config_invalid_base_relocation",
		"Unable to load base relocation entry." },
	{ "pe_load_config_invalid_dynamic_relocation_block",
		"Unable to load dynamic relocation block." },
	{ "pe_load_config_invalid_arm64x_relocation_entry",
		"Unable to load ARM64X relocation entry." },
	{ "pe_load_config_invalid_arm64x_dynamic_relocation_copy_data_data",
		"Unable to load ARM64X dynamic relocation (copy data) data." },
	{ "pe_load_config_invalid_arm64x_dynamic_relocation_add_delta_entry",
		"Unable to load ARM64X dynamic relocation (add delta) entry." },
	{ "pe_load_config_invalid_security_cookie_va",
		"Invalid security cookie virtual address." },
	{ "pe_load_config_invalid_cf_guard_table_function_count",
		"Invalid CF (control flow) guard table function count." },
	{ "pe_load_config_invalid_guard_export_suppression_table_size",
		"Invalid CF (control flow) guard export suppression table size." },
	{ "pe_load_config_invalid_guard_export_suppression_table_function_count",
		"Invalid CF (control flow) guard export suppression table function count." },
	{ "pe_load_config_unsorted_guard_export_suppression_table",
		"Unsorted CF (control flow) guard export suppression table." },
	{ "pe_load_config_invalid_guard_longjump_table_size",
		"Invalid CF (control flow) guard longjump table size." },
	{ "pe_load_config_invalid_guard_longjump_table_function_count",
		"Invalid CF (control flow) guard longjump table function count." },
	{ "pe_load_config_unsorted_guard_longjump_table",
		"Unsorted CF (control flow) guard longjump table." },
	{ "pe_load_config_invalid_guard_cf_check_function_va",
		"Invalid CF (control flow) guard check function virtual address." },
	{ "pe_load_config_invalid_guard_cf_dispatch_function_va",
		"Invalid CF (control flow) guard dispatch function virtual address." },
	{ "pe_load_config_invalid_chpe_range_entry_count",
		"Invalid CHPE (hybrid PE) range entry count." },
	{ "pe_load_config_invalid_chpe_entry_address_or_size",
		"Invalid CHPE (hybrid PE) entry address or size." },
	{ "pe_load_config_invalid_volatile_metadata_access_rva_table_entry_count",
		"Invalid volatile metadata access RVA table entry count." },
	{ "pe_load_config_invalid_volatile_metadata_range_table_entry_count",
		"Invalid volatile metadata range table entry count." },
	{ "pe_load_config_invalid_ehcont_target_rvas",
		"Unable to load exception handling continuation targets RVAs." },
	{ "pe_load_config_invalid_ehcont_targets_count",
		"Invalid exception handling continuation targets count." },

	{ "pe_resources_invalid_directory_size", "Invalid resource directory size." },
	{ "pe_resources_invalid_resource_directory",
		"Unable to read resource directory structures." },
	{ "pe_resources_invalid_resource_directory_number_of_entries",
		"Invalid resource directory number of entries." },
	{ "pe_resources_invalid_resource_directory_entry",
		"Unable to read resource directory entry." },
	{ "pe_resources_invalid_resource_directory_entry_name",
		"Unable to read resource directory entry name." },
	{ "pe_resources_invalid_number_of_named_and_id_entries",
		"Invalid number of named/ID entries in resource directory." },
	{ "pe_resources_invalid_resource_data_entry",
		"Unable to read resource directory data entry." },
	{ "pe_resources_invalid_resource_data_entry_raw_data",
		"Unable to load resource directory data entry data." },
	{ "pe_resources_unsorted_entries",
		"Resource directory has unsorted entries." },
	{ "pe_resources_duplicate_entries",
		"Resource directory has duplicate entries with the same path (type/name or id /language)." },

	{ "pe_security_invalid_directory", "Unable to read security directory, its data is not valid." },
	{ "pe_security_invalid_entry",
		"Security directory contains one or more invalid entries which are not readable." },
	{ "pe_security_invalid_certificate_data",
		"Unable to read security directory certificate data." },
	{ "pe_security_invalid_entry_size",
		"Security directory contains one or more invalid entries with invalid entry size." },
	{ "pe_security_invalid_directory_size",
		"Invalid security directory size." },
	{ "pe_security_unaligned_directory",
		"Security directory is not aligned." },
	{ "pe_security_too_many_entries",
		"There are too many entries in the security directory." },

	{ "pe_authenticode_signature_hash_and_digest_algorithm_mismatch",
		"Authenticode signer digest algorithm and the hash algorithm specified "
		"in the signer digest encryption algorithm info do not match." },
	{ "pe_authenticode_unsupported_digest_algorithm",
		"Authenticode digest algorithm is not supported." },
	{ "pe_authenticode_unsupported_digest_encryption_algorithm",
		"Authenticode signature algorithm is not supported." },
	{ "pe_authenticode_pkcs7_invalid_signed_data_oid",
		"Invalid PKCS7 signed data OID." },
	{ "pe_authenticode_pkcs7_invalid_signed_data_version",
		"Invalid PKCS7 signed data version." },
	{ "pe_authenticode_pkcs7_invalid_signer_count",
		"Invalid PKCS7 signer count." },
	{ "pe_authenticode_pkcs7_non_matching_digest_algorithm",
		"PKCS7 signer and content info digest algorithms do not match." },
	{ "pe_authenticode_pkcs7_invalid_signer_info_version",
		"Invalid PKCS7 signer info version." },
	{ "pe_authenticode_pkcs7_absent_message_digest",
		"PKCS7 message digest attribute is absent." },
	{ "pe_authenticode_pkcs7_invalid_message_digest",
		"PKCS7 message digest attribute format is not valid." },
	{ "pe_authenticode_pkcs7_absent_content_type",
		"PKCS7 content type attribute is absent." },
	{ "pe_authenticode_pkcs7_invalid_content_type",
		"PKCS7 content type attribute format is not valid." },
	{ "pe_authenticode_pkcs7_invalid_signing_time",
		"PKCS7 signing time attribute format is not valid." },
	{ "pe_authenticode_invalid_content_info_oid",
		"Invalid Authenticode signed data content info OID." },
	{ "pe_authenticode_invalid_type_value_type",
		"Invalid Authenticode SpcIndirectDataContent type." },
	{ "pe_authenticode_non_matching_type_value_digest_algorithm",
		"Authenticode SpcIndirectDataContent digest algorithm and signer info digest algorithm do not match." },
	{ "pe_authenticode_invalid_page_hash_format",
		"Invalid Authenticode page hashes format." },
	{ "pe_authenticode_invalid_image_format_for_hashing",
		"Executable image format is not suitable for hashing." },
	{ "pe_authenticode_image_security_directory_has_errors",
		"Executable image security directory has errors." },
	{ "pe_authenticode_invalid_authenticode_signature_format",
		"Authenticode signature format is not valid." },
	{ "pe_authenticode_invalid_tst_info_version",
		"Authenticode timestamp counter-signature TST version if not valid." },
	{ "pe_authenticode_invalid_tst_info_accuracy_value",
		"Authenticode timestamp counter-signature TST accuracy value is not valid." },
	{ "pe_authenticode_duplicate_attribute_oid",
		"Authenticode signature has duplicate attribute OIDs." },
	{ "pe_authenticode_absent_authenticated_attributes",
		"Authenticode signature lacks authenticated attributes." },
	{ "pe_authenticode_absent_certificates",
		"Authenticode signature lacks certificates." },
	{ "pe_authenticode_duplicate_certificates",
		"Authenticode signature has duplicate certificates." },
	{ "pe_authenticode_absent_signing_cert",
		"Authenticode signature signing certificate is absent." },
	{ "pe_authenticode_absent_signing_cert_issuer_and_sn",
		"Authenticode signature signer does not specify signing certificate issuer and/or serial number." },
	{ "pe_authenticode_unable_to_verify_signature",
		"Unable to verify Authenticode signature." },
});

} //namespace bv::resource::en
