#include "binary_valentine/core/rule_selector.h"

#include <cstddef>
#include <unordered_map>
#include <utility>
#include <variant>

#include "binary_valentine/output/format_args_helper.h"

namespace bv::core
{

bool rule_selector::is_enabled(const output::rule_report_base& report) const
{
	if (excluded_categories_[static_cast<std::size_t>(
		report.get_report_category())])
	{
		return false;
	}

	if (excluded_levels_[static_cast<std::size_t>(
		report.get_report_level())])
	{
		return false;
	}

	const bool has_uid = selected_report_uids_.contains(report.get_uid());
	return (mode_ == rule_selection_mode::include_selected && has_uid)
		|| (mode_ == rule_selection_mode::exclude_selected && !has_uid);
}

bool rule_selector::is_enabled(const output::rule_report_base& report,
	std::span<const output::arg_type> args,
	std::span<const char* const> arg_names,
	const output::exception_formatter& formatter) const
{
	if (!is_enabled(report))
		return false;

	auto it = report_selectors_.find(report.get_uid());
	if (it == report_selectors_.end())
		return true;

	return it->second.is_enabled(args, arg_names, formatter);
}

namespace
{

bool arg_matches(const output::arg_type& arg,
	const std::regex& regex,
	const output::exception_formatter& formatter)
{
	output::arg_value_type value;
	output::format_args_helper::format_as_string(
		arg, formatter, value);

	if (const auto* str = std::get_if<std::string>(&value); str)
	{
		if (std::regex_search(*str, regex))
			return true;
	}
	else
	{
		const auto& vec = std::get<
			output::multiple_value_arg_type>(value);
		for (const auto& [name, nested_str] : vec)
		{
			if (std::regex_search(nested_str, regex))
				return true;
		}
	}

	return false;
}

bool any_matches(
	std::span<const output::arg_type> args,
	std::span<const char* const> arg_names,
	const output::exception_formatter& formatter,
	const report_selector::arg_regex_map& arg_name_to_arg)
{
	for (std::size_t i = 0; i != arg_names.size(); ++i)
	{
		auto it = arg_name_to_arg.find(
			std::string_view(arg_names[i]));
		if (it == arg_name_to_arg.end())
			continue;

		if (arg_matches(args[i], it->second.first, formatter))
			return true;
	}

	return false;
}

bool all_match(
	std::span<const output::arg_type> args,
	std::span<const char* const> arg_names,
	const output::exception_formatter& formatter,
	const report_selector::arg_regex_map& arg_name_to_arg)
{
	if (arg_names.size() < arg_name_to_arg.size())
		return false;

	std::unordered_map<std::string_view,
		std::reference_wrapper<const output::arg_type>> name_to_arg;
	name_to_arg.reserve(arg_names.size());
	for (std::size_t i = 0; i != arg_names.size(); ++i)
		name_to_arg.emplace(arg_names[i], args[i]);

	if (name_to_arg.size() < arg_name_to_arg.size())
		return false;

	for (const auto& [arg_name, regex] : arg_name_to_arg)
	{
		auto it = name_to_arg.find(arg_name);
		if (it == name_to_arg.end())
			return false;

		if (!arg_matches(it->second, regex.first, formatter))
			return false;
	}

	return true;
}

} //namespace

bool report_selector::is_enabled(
	std::span<const output::arg_type> args,
	//TODO: this span can be changed to span<string_view> everywhere,
	//but fmt library fmt::arg works with const char* only.
	std::span<const char* const> arg_names,
	const output::exception_formatter& formatter) const
{
	if (arg_name_to_arg_.empty())
		return true;

	bool matches = false;
	if (aggregation_ == aggregation_mode::any)
		matches = any_matches(args, arg_names, formatter, arg_name_to_arg_);
	else
		matches = all_match(args, arg_names, formatter, arg_name_to_arg_);

	return (matches && mode_ == rule_selection_mode::include_selected)
		|| (!matches && mode_ == rule_selection_mode::exclude_selected);
}

} //namespace bv::core
