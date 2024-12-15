#include "binary_valentine/core/user_error.h"

#include <system_error>

namespace
{

std::string_view get_error_string_id_impl(int ev) noexcept
{
	using enum bv::core::user_errc;
	switch (static_cast<bv::core::user_errc>(ev))
	{
	case unable_to_read_file:
		return "user_error_unable_to_read_file";
	case invalid_regex:
		return "user_error_invalid_regex";
	case invalid_root_path:
		return "user_error_invalid_root_path";
	case invalid_thread_count:
		return "user_error_invalid_thread_count";
	case ambiguous_preload_limit_settings:
		return "user_error_ambiguous_preload_limit_settings";
	case invalid_max_concurrent_tasks_value:
		return "user_error_invalid_max_concurrent_tasks_value";
	case invalid_max_loaded_targets_size_value:
		return "user_error_invalid_max_loaded_targets_size_value";
	case invalid_combined_analysis_value:
		return "user_error_invalid_combined_analysis_value";
	case multiple_xml_elements:
		return "user_error_multiple_xml_elements";
	case unsupported_selection_mode:
		return "user_error_unsupported_selection_mode";
	case unsupported_report_id:
		return "user_error_unsupported_report_id";
	case duplicate_report_id:
		return "user_error_duplicate_report_id";
	case unsupported_report_level:
		return "user_error_unsupported_report_level";
	case unsupported_report_category:
		return "user_error_unsupported_report_category";
	case absent_report_filter_report_attribute:
		return "user_error_absent_report_filter_report_attribute";
	case duplicate_report_filter:
		return "user_error_duplicate_report_filter";
	case duplicate_included_excluded_reports:
		return "user_error_duplicate_included_excluded_reports";
	case unsupported_report_list_tag:
		return "user_error_unsupported_report_list_tag";
	case duplicate_excluded_levels:
		return "user_error_duplicate_excluded_levels";
	case unsupported_excluded_levels_list_tag:
		return "user_error_unsupported_excluded_levels_list_tag";
	case duplicate_excluded_level:
		return "user_error_duplicate_excluded_level";
	case duplicate_excluded_categories:
		return "user_error_duplicate_excluded_categories";
	case unsupported_excluded_categories_list_tag:
		return "user_error_unsupported_excluded_categories_list_tag";
	case duplicate_excluded_category:
		return "user_error_duplicate_excluded_category";
	case unsupported_rule_selector_tag:
		return "user_error_unsupported_rule_selector_tag";
	case unsupported_aggregation_mode:
		return "user_error_unsupported_aggregation_mode";
	case unsupported_report_filter_tag:
		return "user_error_unsupported_report_filter_tag";
	case absent_report_filter_arg:
		return "user_error_absent_report_filter_arg";
	case invalid_report_filter:
		return "user_error_invalid_report_filter";
	case no_targets_specified:
		return "user_error_no_targets_specified";
	case invalid_recursive_attribute_value:
		return "user_error_invalid_recursive_attribute_value";
	case unsupported_targets_tag:
		return "user_error_unsupported_targets_tag";
	case absent_target_path:
		return "user_error_absent_target_path";
	case unsupported_filter_tag:
		return "user_error_unsupported_filter_tag";
	case invalid_target_filter:
		return "user_error_invalid_target_filter";
	case unsupported_reports_tag:
		return "user_error_unsupported_reports_tag";
	case empty_report_path:
		return "user_error_empty_report_path";
	case unable_to_load_xml:
		return "user_error_unable_to_load_xml";
	case no_reports_specified:
		return "user_error_no_reports_specified";
	case help_requested:
		return "user_error_help_requested";
	case reports_info_requested:
		return "user_error_reports_info_requested";
	case no_options_allowed_with_config:
		return "user_error_no_options_allowed_with_config";
	case invalid_target_path:
		return "user_error_invalid_target_path";
	case invalid_report_path:
		return "user_error_invalid_report_path";
	case invalid_project_path:
		return "user_error_invalid_project_path";
	case unable_to_write_file:
		return "user_error_unable_to_write_file";
	case duplicate_output_extra_option:
		return "user_error_duplicate_output_extra_option";
	case unable_to_load_html_report_template_file:
		return "user_error_unable_to_load_html_report_template_file";
	case unable_to_write_html_report:
		return "user_error_unable_to_write_html_report";
	default:
		return {};
	}
}

struct user_error_category : std::error_category
{
	const char* name() const noexcept override
	{
		return "bv/user_error";
	}

	std::string message(int ev) const override
	{
		return std::string(get_error_string_id_impl(ev));
	}
};

const user_error_category user_error_category_instance;

} //namespace

namespace bv::core
{

std::error_code make_error_code(user_errc e) noexcept
{
	return { static_cast<int>(e), user_error_category_instance };
}

} //namespace bv::core
