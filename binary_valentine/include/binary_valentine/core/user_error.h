#pragma once

#include <system_error>
#include <type_traits>

#include "binary_valentine/core/localizable_error.h"

namespace bv::core
{

class user_error : public localizable_error
{
public:
	using localizable_error::localizable_error;
};

enum class user_errc
{
	unable_to_read_file = 1,
	invalid_regex,
	invalid_root_path,
	invalid_thread_count,
	ambiguous_preload_limit_settings,
	invalid_max_concurrent_tasks_value,
	invalid_max_loaded_targets_size_value,
	invalid_combined_analysis_value,
	multiple_xml_elements,
	unsupported_selection_mode,
	unsupported_report_id,
	duplicate_report_id,
	unsupported_report_level,
	unsupported_report_category,
	absent_report_filter_report_attribute,
	duplicate_report_filter,
	duplicate_included_excluded_reports,
	unsupported_report_list_tag,
	duplicate_excluded_levels,
	unsupported_excluded_levels_list_tag,
	duplicate_excluded_level,
	duplicate_excluded_categories,
	unsupported_excluded_categories_list_tag,
	duplicate_excluded_category,
	unsupported_rule_selector_tag,
	unsupported_aggregation_mode,
	unsupported_report_filter_tag,
	absent_report_filter_arg,
	invalid_report_filter,
	no_targets_specified,
	invalid_recursive_attribute_value,
	unsupported_targets_tag,
	absent_target_path,
	unsupported_filter_tag,
	invalid_target_filter,
	unsupported_reports_tag,
	empty_report_path,
	unable_to_load_xml,
	no_reports_specified,
	help_requested,
	reports_info_requested,
	no_options_allowed_with_config
};

std::error_code make_error_code(user_errc) noexcept;

} //namespace bv::core

namespace std
{
template<>
struct is_error_code_enum<bv::core::user_errc> : true_type {};
} //namespace std
