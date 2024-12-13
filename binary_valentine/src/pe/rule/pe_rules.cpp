#include "binary_valentine/pe/rule/pe_rules.h"

#include "binary_valentine/pe/rule/ansi_import_rule.h"
#include "binary_valentine/pe/rule/application_manifest_rule.h"
#include "binary_valentine/pe/rule/authenticode_rule.h"
#include "binary_valentine/pe/rule/cet_rule.h"
#include "binary_valentine/pe/rule/checksum_rule.h"
#include "binary_valentine/pe/rule/cross_resource_version_info_rule.h"
#include "binary_valentine/pe/rule/directory_sections_rule.h"
#include "binary_valentine/pe/rule/debug_directory_rule.h"
#include "binary_valentine/pe/rule/debug_directory_format_rule.h"
#include "binary_valentine/pe/rule/dotnet_header_format_rule.h"
#include "binary_valentine/pe/rule/export_directory_format_rule.h"
#include "binary_valentine/pe/rule/export_rule.h"
#include "binary_valentine/pe/rule/flow_guards_rule.h"
#include "binary_valentine/pe/rule/image_errors_rule.h"
#include "binary_valentine/pe/rule/import_format_rule.h"
#include "binary_valentine/pe/rule/load_config_directory_format_rule.h"
#include "binary_valentine/pe/rule/load_config_flags_rule.h"
#include "binary_valentine/pe/rule/main_icon_rule.h"
#include "binary_valentine/pe/rule/manifest_assembly_version_rule.h"
#include "binary_valentine/pe/rule/manifest_format_rule.h"
#include "binary_valentine/pe/rule/not_recommended_import_rule.h"
#include "binary_valentine/pe/rule/pe_file_format_rule.h"
#include "binary_valentine/pe/rule/relocs_rule.h"
#include "binary_valentine/pe/rule/resource_directory_format_rule.h"
#include "binary_valentine/pe/rule/rich_data_rule.h"
#include "binary_valentine/pe/rule/sections_rule.h"
#include "binary_valentine/pe/rule/safeseh_rule.h"
#include "binary_valentine/pe/rule/simple_flags_rule.h"
#include "binary_valentine/pe/rule/security_cookie_rule.h"
#include "binary_valentine/pe/rule/security_directory_format_rule.h"
#include "binary_valentine/pe/rule/vc_feature_rule.h"
#include "binary_valentine/pe/rule/version_info_rule.h"
#include "binary_valentine/pe/rule/version_info_format_rule.h"

#include "binary_valentine/pe/combined_rule/combined_cross_signature_check_rule.h"
#include "binary_valentine/pe/combined_rule/combined_dll_import_names_case_rule.h"
#include "binary_valentine/pe/combined_rule/combined_version_info_rule.h"

namespace bv::pe
{

void pe_rules_factory::add_pe_rules(core::rule_list& rules,
	core::combined_rule_list& combined_rules,
	core::value_provider_interface& shared_values)
{
	debug_directory_rule_factory::add_rule(rules, shared_values);
	flow_guards_rule_factory::add_rule(rules, shared_values);
	sections_rule_factory::add_rule(rules, shared_values);
	relocs_rule_factory::add_rule(rules, shared_values);
	simple_flags_rule_factory::add_rule(rules, shared_values);
	safeseh_rule_factory::add_rule(rules, shared_values);
	security_cookie_rule_factory::add_rule(rules, shared_values);
	pe_file_format_rule_factory::add_rule(rules, shared_values);
	version_info_rule_factory::add_rule(rules, shared_values);
	export_rule_factory::add_rule(rules, shared_values);
	cross_resource_version_info_rule_factory::add_rule(rules, shared_values);
	version_info_format_rule_factory::add_rule(rules, shared_values);
	main_icon_rule_factory::add_rule(rules, shared_values);
	cet_rule_factory::add_rule(rules, shared_values);
	not_recommended_import_rule_factory::add_rule(rules, shared_values);
	ansi_import_rule_factory::add_rule(rules, shared_values);
	directory_sections_rule_factory::add_rule(rules, shared_values);
	checksum_rule_factory::add_rule(rules, shared_values);
	rich_data_rule_factory::add_rule(rules, shared_values);
	application_manifest_rule_factory::add_rule(rules, shared_values);
	image_errors_rule_factory::add_rule(rules, shared_values);
	manifest_format_rule_factory::add_rule(rules, shared_values);
	load_config_flags_rule_factory::add_rule(rules, shared_values);
	dotnet_header_format_rule_factory::add_rule(rules, shared_values);
	debug_directory_format_rule_factory::add_rule(rules, shared_values);
	import_format_rule_factory::add_rule(rules, shared_values);
	export_directory_format_rule_factory::add_rule(rules, shared_values);
	load_config_directory_format_rule_factory::add_rule(rules, shared_values);
	resource_directory_format_rule_factory::add_rule(rules, shared_values);
	vc_feature_rule_factory::add_rule(rules, shared_values);
	manifest_assembly_version_rule_factory::add_rule(rules, shared_values);
	authenticode_rule_factory::add_rule(rules, shared_values);
	security_directory_format_rule_factory::add_rule(rules, shared_values);
	
	combined_version_info_rule_factory::add_rule(combined_rules, shared_values);
	combined_dll_import_names_case_rule_factory::add_rule(combined_rules, shared_values);
	combined_cross_signature_check_rule_factory::add_rule(combined_rules, shared_values);
}

} //namespace bv::pe
