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
	core::combined_rule_list& combined_rules)
{
	debug_directory_rule_factory::add_rule(rules);
	flow_guards_rule_factory::add_rule(rules);
	sections_rule_factory::add_rule(rules);
	relocs_rule_factory::add_rule(rules);
	simple_flags_rule_factory::add_rule(rules);
	safeseh_rule_factory::add_rule(rules);
	security_cookie_rule_factory::add_rule(rules);
	pe_file_format_rule_factory::add_rule(rules);
	version_info_rule_factory::add_rule(rules);
	export_rule_factory::add_rule(rules);
	cross_resource_version_info_rule_factory::add_rule(rules);
	version_info_format_rule_factory::add_rule(rules);
	main_icon_rule_factory::add_rule(rules);
	cet_rule_factory::add_rule(rules);
	not_recommended_import_rule_factory::add_rule(rules);
	ansi_import_rule_factory::add_rule(rules);
	directory_sections_rule_factory::add_rule(rules);
	checksum_rule_factory::add_rule(rules);
	rich_data_rule_factory::add_rule(rules);
	application_manifest_rule_factory::add_rule(rules);
	image_errors_rule_factory::add_rule(rules);
	manifest_format_rule_factory::add_rule(rules);
	load_config_flags_rule_factory::add_rule(rules);
	dotnet_header_format_rule_factory::add_rule(rules);
	debug_directory_format_rule_factory::add_rule(rules);
	import_format_rule_factory::add_rule(rules);
	export_directory_format_rule_factory::add_rule(rules);
	load_config_directory_format_rule_factory::add_rule(rules);
	resource_directory_format_rule_factory::add_rule(rules);
	vc_feature_rule_factory::add_rule(rules);
	manifest_assembly_version_rule_factory::add_rule(rules);
	authenticode_rule_factory::add_rule(rules);
	security_directory_format_rule_factory::add_rule(rules);
	
	combined_version_info_rule_factory::add_rule(combined_rules);
	combined_dll_import_names_case_rule_factory::add_rule(combined_rules);
	combined_cross_signature_check_rule_factory::add_rule(combined_rules);
}

} //namespace bv::pe
