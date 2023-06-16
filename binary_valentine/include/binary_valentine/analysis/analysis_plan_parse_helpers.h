#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/core/user_error.h"

namespace bv::analysis
{

class analysis_plan;
class target_filter;

class analysis_plan_parse_helpers final
{
public:
	analysis_plan_parse_helpers() = delete;
	
	static void parse_root_path(std::string_view root_path, analysis_plan& plan);
	static void parse_thread_count(std::string_view thread_count, analysis_plan& plan);
	static void parse_combined_analysis_option(std::string_view enabled, analysis_plan& plan);
	static void parse_preload_limit(std::optional<std::string_view> max_concurrent_tasks_str,
		std::optional<std::string_view> max_loaded_targets_size_str, analysis_plan& plan);
	static void parse_report_filter_regex(std::string_view arg, std::string_view regex,
		core::report_selector& selector);
	static void parse_target_filter_regex(
		std::string_view regex, bool include, target_filter& target);
	static void add_excluded_level(std::string_view level, core::rule_selector& selector);
	static void add_excluded_category(std::string_view category, core::rule_selector& selector);
	static void add_selected_report(std::string_view report_uid, const core::rule_list& rules,
		const core::combined_rule_list& combined_rules, core::rule_selector& selector);

	[[nodiscard]]
	static core::rule_selection_mode parse_selection_mode(std::string_view mode);
	[[nodiscard]]
	static core::aggregation_mode parse_aggregation_mode(std::string_view mode);
	[[nodiscard]]
	static output::report_level parse_report_level(std::string_view value);
	[[nodiscard]]
	static output::report_category parse_report_category(std::string_view value);
	[[nodiscard]]
	static output::report_uid get_report_uid(std::string_view string_uid,
		const core::rule_list& rules, const core::combined_rule_list& combined_rules);

	template<core::user_errc ErrorCode>
	[[nodiscard]]
	static bool to_bool(std::string_view str)
	{
		if (str == "1" || str == "true")
			return true;
		if (str == "0" || str == "false")
			return false;

		throw core::user_error(ErrorCode,
			core::user_error::arg_type("value", std::string(str)));
	}
};

} //namespace bv::analysis
