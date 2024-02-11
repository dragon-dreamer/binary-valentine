#include "binary_valentine/analysis/analysis_plan.h"

#include <utility>

#include "binary_valentine/core/user_error.h"
#include "binary_valentine/string/encoding.h"

namespace bv::analysis
{

namespace
{
bool include(const std::string& utf8_canonical_path,
	const std::vector<std::regex>& include_regex)
{
	if (include_regex.empty())
		return true;

	for (const auto& regex : include_regex)
	{
		if (std::regex_search(utf8_canonical_path, regex))
			return true;
	}
	return false;
}

bool exclude(const std::string& utf8_canonical_path,
	const std::vector<std::regex>& exclude_regex)
{
	if (exclude_regex.empty())
		return false;

	for (const auto& regex : exclude_regex)
	{
		if (std::regex_search(utf8_canonical_path, regex))
			return true;
	}
	return false;
}

void add_regex(std::string_view regex,
	std::vector<std::regex>& regex_list,
	std::vector<std::string>& regex_string_list)
{
	try
	{
		regex_list.emplace_back(regex.begin(), regex.end());
		regex_string_list.emplace_back(regex.begin(), regex.end());
	}
	catch (const std::regex_error&)
	{
		std::throw_with_nested(core::user_error(core::user_errc::invalid_regex,
			core::user_error::arg_type("regex", std::string(regex))));
	}
}
} //namespace

bool target_filter::satisfies(const std::filesystem::path& path) const
{
	if (!include_regex_.empty() || !exclude_regex_.empty())
	{
		auto utf8_path = string::to_utf8(path.native());
		if (!include(utf8_path, include_regex_)
			|| exclude(utf8_path, exclude_regex_))
		{
			return false;
		}
	}
	return true;
}

void target_filter::add_include_regex(std::string_view regex)
{
	add_regex(regex, include_regex_, include_regex_strings_);
}

void target_filter::add_exclude_regex(std::string_view regex)
{
	add_regex(regex, exclude_regex_, exclude_regex_strings_);
}

plan_target::plan_target(std::string_view path)
	: path_(string::utf8_to<std::filesystem::path::string_type>
		::convert(path))
{
}

plan_target::plan_target(std::filesystem::path&& path)
	: path_(std::move(path))
{
}

void analysis_plan::set_root_path(std::filesystem::path&& path)
{
	root_path_ = std::move(path);
}

void analysis_plan::set_root_path(std::string_view utf8_path)
{
	root_path_ = string::utf8_to<std::filesystem::path::string_type>
		::convert(utf8_path);
}

void analysis_plan::set_progress_report(
	std::shared_ptr<progress::progress_report_interface>&& report) noexcept
{
	progress_report_ = std::move(report);
}

void analysis_plan::add_realtime_report_creator(
	output::realtime_report_creator_type&& creator)
{
	report_creators_.emplace_back(std::move(creator));
}

void analysis_plan::set_custom_in_memory_report_creator(
	output::in_memory_report_creator_type&& creator)
{
	in_memory_report_creator_ = std::move(creator);
}

result_report_file::result_report_file(std::string_view path, result_report_file_type type)
	: path_(string::utf8_to<std::filesystem::path::string_type>
		::convert(path))
	, type_(type)
{
}

result_report_file::result_report_file(
	std::filesystem::path&& path, result_report_file_type type)
	: path_(std::move(path))
	, type_(type)
{
}

} //namespace bv::analysis
