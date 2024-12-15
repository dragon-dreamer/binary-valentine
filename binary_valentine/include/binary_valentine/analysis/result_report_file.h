#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>

#include "binary_valentine/core/transparent_hash.h"

namespace bv::analysis
{

enum class result_report_file_type
{
	text,
	sarif,
};

class [[nodiscard]] result_report_file
{
public:
	using options_map_type = std::unordered_map<std::string, std::string,
		core::transparent_hash, std::equal_to<>>;

public:
	explicit result_report_file(std::string_view path, result_report_file_type type);
	explicit result_report_file(std::filesystem::path&& path, result_report_file_type type);

	[[nodiscard]]
	const std::filesystem::path& get_path() const noexcept
	{
		return path_;
	}

	[[nodiscard]]
	result_report_file_type get_type() const noexcept
	{
		return type_;
	}

	bool add_extra_option(std::string name, std::string value)
	{
		return extra_options_.try_emplace(
			std::move(name), std::move(value)).second;
	}

	[[nodiscard]] const options_map_type& get_extra_options() const noexcept
	{
		return extra_options_;
	}

private:
	std::filesystem::path path_;
	result_report_file_type type_;
	options_map_type extra_options_;
};

} //namespace bv::analysis
