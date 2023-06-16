#pragma once

#include <array>
#include <cstddef>
#include <regex>
#include <span>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/rule_report.h"

namespace bv::output
{
class exception_formatter;
} //namespace bv::output

namespace bv::core
{

enum class rule_selection_mode
{
	exclude_selected,
	include_selected
};

enum class aggregation_mode
{
	all,
	any
};

class [[nodiscard]] report_selector
{
public:
	[[nodiscard]]
	bool is_enabled(
		std::span<const output::arg_type> args,
		std::span<const char* const> arg_names,
		const output::exception_formatter& formatter) const;

	void set_selection_mode(rule_selection_mode mode) noexcept
	{
		mode_ = mode;
	}

	void set_aggregation_mode(aggregation_mode mode) noexcept
	{
		aggregation_ = mode;
	}

	template<typename ArgName, typename Regex>
	void add_regex(ArgName&& arg_name, Regex&& regex)
	{
		arg_name_to_arg_.emplace(std::forward<ArgName>(arg_name),
			std::forward<Regex>(regex));
	}

private:
	aggregation_mode aggregation_{ aggregation_mode::any };
	rule_selection_mode mode_{ rule_selection_mode::exclude_selected };
	std::unordered_map<std::string, std::regex, transparent_hash,
		std::equal_to<>> arg_name_to_arg_;
};

class [[nodiscard]] rule_selector
{
public:
	[[nodiscard]]
	bool is_enabled(const output::rule_report_base& report,
		std::span<const output::arg_type> args,
		std::span<const char* const> arg_names,
		const output::exception_formatter& formatter) const;
	[[nodiscard]]
	bool is_enabled(const output::rule_report_base& report) const;
	[[nodiscard]]
	bool is_enabled(output::report_level level) const
	{
		return !excluded_levels_[static_cast<std::size_t>(level)];
	}

	void set_mode(rule_selection_mode mode) noexcept
	{
		mode_ = mode;
	}

	bool add_selected_report_uid(output::report_uid uid)
	{
		return selected_report_uids_.emplace(uid).second;
	}

	bool exclude_report_level(output::report_level level, bool exclude = true) noexcept
	{
		return std::exchange(excluded_levels_[static_cast<std::size_t>(level)], exclude);
	}

	bool exclude_report_category(output::report_category category, bool exclude = true) noexcept
	{
		return std::exchange(excluded_categories_[static_cast<std::size_t>(category)], exclude);
	}

	[[nodiscard]]
	std::pair<std::reference_wrapper<report_selector>, bool>
		get_or_create_report_selector(output::report_uid uid)
	{
		auto it = report_selectors_.find(uid);
		if (it != report_selectors_.end())
			return { it->second, false };

		return { report_selectors_[uid], true };
	}

private:
	std::unordered_set<output::report_uid, transparent_hash,
		std::equal_to<>> selected_report_uids_;
	std::unordered_map<output::report_uid, report_selector, transparent_hash,
		std::equal_to<>> report_selectors_;
	rule_selection_mode mode_{ rule_selection_mode::exclude_selected };
	std::array<bool, static_cast<std::size_t>(
		output::report_level::max)> excluded_levels_{};
	std::array<bool, static_cast<std::size_t>(
		output::report_category::max)> excluded_categories_{};
};

} //namespace bv::core
