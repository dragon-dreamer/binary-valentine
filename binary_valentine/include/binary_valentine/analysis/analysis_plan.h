#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "binary_valentine/core/rule_selector.h"

namespace bv::progress { class progress_report_interface; }

namespace bv::analysis
{

class [[nodiscard]] target_filter
{
public:
	[[nodiscard]]
	bool satisfies(const std::filesystem::path& path) const;

public:
	void add_include_regex(std::string_view regex);
	void add_exclude_regex(std::string_view regex);

private:
	std::vector<std::regex> include_regex_;
	std::vector<std::regex> exclude_regex_;
};

class [[nodiscard]] plan_target
{
public:
	explicit plan_target(std::string_view path);
	explicit plan_target(std::filesystem::path&& path);

	void set_recursive(bool recursive) noexcept
	{
		recursive_ = recursive;
	}

	[[nodiscard]]
	target_filter& get_target_filter() noexcept
	{
		return filter_;
	}

	[[nodiscard]]
	core::rule_selector& emplace_rule_selector()
	{
		return selector_.emplace();
	}

public:
	[[nodiscard]]
	const std::filesystem::path& get_path() const noexcept
	{
		return path_;
	}

	[[nodiscard]]
	bool is_recursive() const noexcept
	{
		return recursive_;
	}

	[[nodiscard]]
	const target_filter& get_target_filter() const noexcept
	{
		return filter_;
	}

	[[nodiscard]]
	const std::optional<core::rule_selector>& get_rule_selector() const noexcept
	{
		return selector_;
	}

private:
	std::filesystem::path path_;
	target_filter filter_;
	std::optional<core::rule_selector> selector_;
	bool recursive_ = true;
};

class [[nodiscard]] combined_analysis_plan
{
public:
	void enable_combined_analysis(bool enable) noexcept
	{
		do_combined_analysis_ = enable;
	}

	[[nodiscard]]
	bool do_combined_analysis() const noexcept
	{
		return do_combined_analysis_;
	}

private:
	bool do_combined_analysis_ = true;
};

struct max_concurrent_tasks
{
	std::uint8_t value = 10u;
};

struct max_loaded_targets_size
{
	std::uint64_t value = 1024 * 1028 * 1024; //1Gb
};

using target_preload_limit_type = std::variant<
	max_concurrent_tasks, max_loaded_targets_size>;

struct result_report_terminal {};
struct result_report_in_memory {};

enum class result_report_file_type
{
	text,
	sarif
};

class [[nodiscard]] result_report_file
{
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

private:
	std::filesystem::path path_;
	result_report_file_type type_;
};

using result_report_type = std::variant<
	result_report_terminal, result_report_in_memory, result_report_file>;

constexpr std::string_view default_language = "en";

class [[nodiscard]] analysis_plan
{
public:
	using thread_count_type = std::uint8_t;

public:
	template<typename... Args>
	plan_target& emplace_target(Args&&... args)
	{
		return targets_.emplace_back(std::forward<Args>(args)...);
	}

	template<typename... Args>
	result_report_type& emplace_output_report(Args&&... args)
	{
		return output_reports_.emplace_back(std::forward<Args>(args)...);
	}

	void set_thread_count(thread_count_type thread_count) noexcept
	{
		thread_count_ = thread_count;
	}

	void enable_combined_analysis(bool enable) noexcept
	{
		combined_plan_.enable_combined_analysis(enable);
	}

	[[nodiscard]]
	core::rule_selector& get_global_rule_selector() noexcept
	{
		return global_selector_;
	}

	[[nodiscard]]
	std::vector<result_report_type>& get_result_reports() noexcept
	{
		return output_reports_;
	}

	template<typename Limit>
	void set_preload_limit(Limit&& limit)
	{
		preload_limit_ = std::forward<Limit>(limit);
	}

	template<typename String>
	void set_language(String&& language)
	{
		language_ = std::forward<String>(language);
	}
	
	void set_root_path(std::string_view utf8_path);
	void set_root_path(std::filesystem::path&& path);

	void enable_signal_cancellation(bool enable) noexcept
	{
		enable_signal_cancellation_ = enable;
	}

	void set_progress_report(
		std::shared_ptr<progress::progress_report_interface>&& report) noexcept;

public:
	[[nodiscard]]
	bool do_combined_analysis() const noexcept
	{
		return combined_plan_.do_combined_analysis();
	}

	[[nodiscard]]
	const std::vector<plan_target>& get_targets() const noexcept
	{
		return targets_;
	}

	[[nodiscard]]
	thread_count_type get_thread_count() const noexcept
	{
		return thread_count_;
	}

	[[nodiscard]]
	const core::rule_selector& get_global_rule_selector() const noexcept
	{
		return global_selector_;
	}

	[[nodiscard]]
	const std::vector<result_report_type>& get_result_reports() const noexcept
	{
		return output_reports_;
	}

	[[nodiscard]]
	const target_preload_limit_type& get_preload_limit() const noexcept
	{
		return preload_limit_;
	}

	[[nodiscard]]
	const std::string& get_language() const noexcept
	{
		return language_;
	}

	[[nodiscard]]
	const std::filesystem::path& get_root_path() const noexcept
	{
		return root_path_;
	}

	[[nodiscard]]
	bool enable_signal_cancellation() const noexcept
	{
		return enable_signal_cancellation_;
	}

	[[nodiscard]]
	const std::shared_ptr<progress::progress_report_interface>&
		get_progress_report() const noexcept
	{
		return progress_report_;
	}

private:
	std::filesystem::path root_path_;
	std::vector<plan_target> targets_;
	core::rule_selector global_selector_;
	std::vector<result_report_type> output_reports_;
	target_preload_limit_type preload_limit_{ max_loaded_targets_size{} };
	std::string language_{ default_language };
	combined_analysis_plan combined_plan_;
	thread_count_type thread_count_ = 0;
	bool enable_signal_cancellation_ = false;
	std::shared_ptr<progress::progress_report_interface> progress_report_;
};

} //namespace bv::analysis
