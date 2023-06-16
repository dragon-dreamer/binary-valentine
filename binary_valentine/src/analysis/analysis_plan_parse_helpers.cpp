#include "binary_valentine/analysis/analysis_plan_parse_helpers.h"

#include <charconv>
#include <cstdint>
#include <exception>
#include <string>
#include <system_error>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/core/user_error.h"

namespace bv::analysis
{

namespace
{
template<core::user_errc ErrorCode>
std::uint64_t get_size_multiplier(char suffix, std::string_view value)
{
	switch (suffix)
	{
	case 'G':
		return 1024u * 1024u * 1024u;
	case 'M':
		return 1024u * 1024u;
	case 'K':
		return 1024u;
	case 'B':
		return 1u;
	default:
		throw core::user_error(ErrorCode,
			core::user_error::arg_type("value", std::string(value)));
	}
}

template<std::integral T, core::user_errc ErrorCode>
[[nodiscard]]
T to_integer(std::string_view view)
{
	T value;
	auto result = std::from_chars(view.data(), view.data() + view.size(), value);
	if (result.ec != std::errc{} || result.ptr != view.data() + view.size())
	{
		throw core::user_error(ErrorCode,
			core::user_error::arg_type("value", std::string(view)));
	}
	return value;
}

} //namespace

void analysis_plan_parse_helpers::parse_root_path(
	std::string_view root_path, analysis_plan& plan)
{
	try
	{
		plan.set_root_path(root_path);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(core::user_error(core::user_errc::invalid_root_path));
	}
}

void analysis_plan_parse_helpers::parse_thread_count(
	std::string_view thread_count, analysis_plan& plan)
{
	plan.set_thread_count(to_integer<analysis_plan::thread_count_type,
		core::user_errc::invalid_thread_count>(thread_count));
}

void analysis_plan_parse_helpers::parse_preload_limit(
	std::optional<std::string_view> max_concurrent_tasks_str,
	std::optional<std::string_view> max_loaded_targets_size_str,
	analysis_plan& plan)
{
	if (max_concurrent_tasks_str && max_loaded_targets_size_str)
		throw core::user_error(core::user_errc::ambiguous_preload_limit_settings);

	if (max_concurrent_tasks_str)
	{
		auto value = to_integer<decltype(max_concurrent_tasks::value),
			core::user_errc::invalid_max_concurrent_tasks_value>(
				max_concurrent_tasks_str.value());
		if (!value || value < plan.get_thread_count())
		{
			throw core::user_error(core::user_errc::invalid_max_concurrent_tasks_value,
				core::user_error::arg_type("value",
					std::string(max_concurrent_tasks_str.value())));
		}

		plan.set_preload_limit(max_concurrent_tasks{ value });
		return;
	}

	if (!max_loaded_targets_size_str)
		return;

	if (max_loaded_targets_size_str->size() < 2u)
	{
		throw core::user_error(core::user_errc::invalid_max_loaded_targets_size_value,
			core::user_error::arg_type("value", std::string(*max_loaded_targets_size_str)));
	}

	auto value = to_integer<decltype(max_loaded_targets_size::value),
		core::user_errc::invalid_max_loaded_targets_size_value>(
			max_loaded_targets_size_str->substr(0, max_loaded_targets_size_str->size() - 1u));

	if (!value)
	{
		throw core::user_error(core::user_errc::invalid_max_loaded_targets_size_value,
			core::user_error::arg_type("value", std::string(*max_loaded_targets_size_str)));
	}

	auto unscaled_value = value;
	std::uint64_t multiplier = get_size_multiplier<
		core::user_errc::invalid_max_loaded_targets_size_value>(
			max_loaded_targets_size_str->back(), *max_loaded_targets_size_str);
	value *= multiplier;
	if (value / multiplier != unscaled_value)
	{
		throw core::user_error(core::user_errc::invalid_max_loaded_targets_size_value,
			core::user_error::arg_type("value", std::string(*max_loaded_targets_size_str)));
	}

	plan.set_preload_limit(max_loaded_targets_size{ value });
}

void analysis_plan_parse_helpers::parse_combined_analysis_option(
	std::string_view enabled, analysis_plan& plan)
{
	plan.enable_combined_analysis(
		to_bool<core::user_errc::invalid_combined_analysis_value>(enabled));
}

core::rule_selection_mode analysis_plan_parse_helpers::parse_selection_mode(
	std::string_view mode)
{
	if (mode == "include")
	{
		return core::rule_selection_mode::include_selected;
	}
	else if (mode == "exclude")
	{
		return core::rule_selection_mode::exclude_selected;
	}
	else
	{
		throw core::user_error(core::user_errc::unsupported_selection_mode,
			core::user_error::arg_type("mode", std::string(mode)));
	}
}

core::aggregation_mode analysis_plan_parse_helpers::parse_aggregation_mode(
	std::string_view mode)
{
	if (mode == "any")
	{
		return core::aggregation_mode::any;
	}
	else if (mode == "all")
	{
		return core::aggregation_mode::all;
	}
	else
	{
		throw core::user_error(core::user_errc::unsupported_aggregation_mode,
			core::user_error::arg_type("mode", std::string(mode)));
	}
}

output::report_level analysis_plan_parse_helpers::parse_report_level(
	std::string_view value)
{
	if (value == "info")
		return output::report_level::info;
	if (value == "warning")
		return output::report_level::warning;
	if (value == "error")
		return output::report_level::error;
	if (value == "critical")
		return output::report_level::critical;

	throw core::user_error(core::user_errc::unsupported_report_level,
		core::user_error::arg_type("level", std::string(value)));
}

output::report_category analysis_plan_parse_helpers::parse_report_category(
	std::string_view value)
{
	if (value == "system")
		return output::report_category::system;
	if (value == "optimization")
		return output::report_category::optimization;
	if (value == "security")
		return output::report_category::security;
	if (value == "configuration")
		return output::report_category::configuration;
	if (value == "format")
		return output::report_category::format;

	throw core::user_error(core::user_errc::unsupported_report_category,
		core::user_error::arg_type("category", std::string(value)));
}

output::report_uid analysis_plan_parse_helpers::get_report_uid(
	std::string_view string_uid,
	const core::rule_list& rules, const core::combined_rule_list& combined_rules)
{
	auto uid = rules.get_report_uid(string_uid);
	if (!uid)
		uid = combined_rules.get_report_uid(string_uid);
	if (!uid)
	{
		throw core::user_error(core::user_errc::unsupported_report_id,
			core::user_error::arg_type("report_id", std::string(string_uid)));
	}
	return uid;
}

void analysis_plan_parse_helpers::parse_report_filter_regex(
	std::string_view arg, std::string_view regex,
	core::report_selector& selector)
{
	try
	{
		selector.add_regex(arg, std::string(regex));
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(core::user_error(
			core::user_errc::invalid_report_filter,
			core::user_error::arg_type("regex", std::string(regex))));
	}
}

void analysis_plan_parse_helpers::parse_target_filter_regex(
	std::string_view regex, bool include, target_filter& target)
{
	try
	{
		if (include)
			target.add_include_regex(regex);
		else
			target.add_exclude_regex(regex);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(core::user_error(
			core::user_errc::invalid_target_filter,
			core::user_error::arg_type("regex", std::string(regex))));
	}
}

void analysis_plan_parse_helpers::add_excluded_level(
	std::string_view level, core::rule_selector& selector)
{
	if (selector.exclude_report_level(parse_report_level(level)))
	{
		throw core::user_error(core::user_errc::duplicate_excluded_level,
			core::user_error::arg_type("level", std::string(level)));
	}
}

void analysis_plan_parse_helpers::add_excluded_category(
	std::string_view category, core::rule_selector& selector)
{
	if (selector.exclude_report_category(parse_report_category(category)))
	{
		throw core::user_error(core::user_errc::duplicate_excluded_category,
			core::user_error::arg_type("category", std::string(category)));
	}
}

void analysis_plan_parse_helpers::add_selected_report(
	std::string_view report_uid, const core::rule_list& rules,
	const core::combined_rule_list& combined_rules, core::rule_selector& selector)
{
	if (!selector.add_selected_report_uid(get_report_uid(report_uid, rules, combined_rules)))
	{
		throw core::user_error(core::user_errc::duplicate_report_id,
			core::user_error::arg_type("report_id", std::string(report_uid)));
	}
}

} //namespace bv::analysis
