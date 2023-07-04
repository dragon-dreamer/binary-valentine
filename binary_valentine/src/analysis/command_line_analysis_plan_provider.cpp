#include "binary_valentine/analysis/command_line_analysis_plan_provider.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <limits>
#include <memory>
#include <ranges>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/predef/os/windows.h>
#include <boost/program_options.hpp>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/analysis_plan_parse_helpers.h"
#include "binary_valentine/core/user_error.h"
#include "binary_valentine/progress/terminal_progress_report.h"
#include "binary_valentine/string/encoding.h"
#include "binary_valentine/string/resource_provider_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/version.h"

namespace bv::analysis
{

namespace po = boost::program_options;

namespace
{

template<typename Char>
struct options_helper
{
public:
	explicit options_helper(int argc, const Char* const* argv)
		: opts_(get_options_description())
		, argc_(argc)
		, argv_(argv)
	{
	}
	
	[[nodiscard]]
	const po::options_description& get_description() const noexcept
	{
		return opts_;
	}

	[[nodiscard]]
	analysis_plan get_plan(const command_line_analysis_plan_provider
		::config_file_analysis_plan_provider_type& config_file_analysis_plan_provider,
		const core::rule_list& rules,
		const core::combined_rule_list& combined_rules,
		const string::resource_provider_interface& resources) const
	{
		po::variables_map vm;
		po::positional_options_description pos_opts;
		pos_opts.add("target", -1);
		po::store(po::basic_command_line_parser<Char>(argc_, argv_).
			options(opts_).positional(pos_opts).run(), vm);

		if (vm.contains("help"))
			throw core::user_error(core::user_errc::help_requested);
		if (vm.contains("list-reports"))
			throw core::user_error(core::user_errc::reports_info_requested);

		if (auto config_path_it = vm.find("config"); config_path_it != vm.end())
		{
			for (const auto& opt : vm)
			{
				if (!opt.second.defaulted()
					&& opt.first != "config" && opt.first != "with-progress")
				{
					throw core::user_error(core::user_errc::no_options_allowed_with_config);
				}
			}

			const auto& config_path = config_path_it->second.as<std::basic_string<Char>>();
			auto plan = config_file_analysis_plan_provider(to_path(config_path))->get();
			set_plan_common_options(vm, resources, plan);
			return plan;
		}

		po::notify(vm);

		analysis_plan plan;
		plan.enable_combined_analysis(vm["do-combined-analysis"].as<bool>());
		parse_root_path(vm, plan);
		parse_threads(vm, plan);
		parse_preload_limits(vm, plan);
		parse_target(vm, plan);
		parse_output_reports(vm, plan);
		parse_global_selector(vm, rules, combined_rules, plan);
		set_plan_common_options(vm, resources, plan);
		return plan;
	}

private:
	static void set_plan_common_options(const po::variables_map& vm,
		const string::resource_provider_interface& resources, analysis_plan& plan)
	{
		plan.enable_signal_cancellation(true);
		if (vm.contains("with-progress"))
		{
			plan.set_progress_report(std::make_shared<
				progress::terminal_progress_report>(resources));
		}
	}

	static void parse_global_selector(const po::variables_map& vm,
		const core::rule_list& rules,
		const core::combined_rule_list& combined_rules,
		analysis_plan& plan)
	{
		auto& selector = plan.get_global_rule_selector();
		if (auto levels_it = vm.find("exclude-levels"); levels_it != vm.end())
		{
			const auto& levels = levels_it->second.as<
				std::vector<std::basic_string<Char>>>();
			for (const auto& level : levels)
			{
				analysis_plan_parse_helpers::add_excluded_level(
					string::to_utf8(level), selector);
			}
		}

		if (auto categories_it = vm.find("exclude-categories"); categories_it != vm.end())
		{
			const auto& categories = categories_it->second.as<
				std::vector<std::basic_string<Char>>>();
			for (const auto& category : categories)
			{
				analysis_plan_parse_helpers::add_excluded_category(
					string::to_utf8(category), selector);
			}
		}

		auto reports_it = vm.find("include-reports");
		if (reports_it == vm.end())
		{
			reports_it = vm.find("exclude-reports");
		}
		else
		{
			selector.set_mode(core::rule_selection_mode::include_selected);
			if (vm.contains("exclude-reports"))
				throw core::user_error(core::user_errc::duplicate_included_excluded_reports);
		}

		if (reports_it != vm.end())
		{
			const auto& reports = reports_it->second.as<
				std::vector<std::basic_string<Char>>>();
			for (const auto& report : reports)
			{
				analysis_plan_parse_helpers::add_selected_report(
					string::to_utf8(report), rules, combined_rules, selector);
			}
		}
	}

	static void parse_output_reports(const po::variables_map& vm, analysis_plan& plan)
	{
		if (!vm.contains("silent"))
			plan.emplace_output_report(result_report_terminal{});

		if (auto path_it = vm.find("sarif"); path_it != vm.end())
		{
			plan.emplace_output_report(result_report_file(to_path(
				path_it->second.as<std::basic_string<Char>>()),
				result_report_file_type::sarif));
		}

		if (auto path_it = vm.find("text"); path_it != vm.end())
		{
			plan.emplace_output_report(result_report_file(to_path(
				path_it->second.as<std::basic_string<Char>>()),
				result_report_file_type::text));
		}

		if (plan.get_result_reports().empty())
			throw core::user_error(core::user_errc::no_reports_specified);
	}

	static void parse_target(const po::variables_map& vm, analysis_plan& plan)
	{
		auto target_path_it = vm.find("target");
		if (target_path_it == vm.end())
			throw core::user_error(core::user_errc::absent_target_path);

		const auto& target_paths = target_path_it->second.as<
			std::vector<std::basic_string<Char>>>();

		const bool is_recursive = vm["recursive"].as<bool>();
		target_filter filter;
		if (auto regex_it = vm.find("include-regex"); regex_it != vm.end())
		{
			analysis_plan_parse_helpers::parse_target_filter_regex(string::to_utf8(regex_it
				->second.as<std::basic_string<Char>>()), true, filter);
		}

		if (auto regex_it = vm.find("exclude-regex"); regex_it != vm.end())
		{
			analysis_plan_parse_helpers::parse_target_filter_regex(string::to_utf8(regex_it
				->second.as<std::basic_string<Char>>()), false, filter);
		}

		plan.reserve_targets(target_paths.size());
		for (const auto& path : target_paths
			| std::views::take(target_paths.size() - 1u))
		{
			add_target(plan, is_recursive, path, filter);
		}

		add_target(plan, is_recursive, target_paths.back(), std::move(filter));
	}

	template<typename Path, typename Filter>
	static void add_target(analysis_plan& plan, bool is_recursive,
		const Path& path, Filter&& filter)
	{
		plan_target& target = plan.emplace_target(to_path(path));
		target.set_recursive(is_recursive);
		target.get_target_filter() = std::forward<Filter>(filter);
	}
	
	static void parse_preload_limits(const po::variables_map& vm, analysis_plan& plan)
	{
		auto max_loaded_targets_size_it = vm.find("max-loaded-targets-size");
		auto max_concurrent_tasks_it = vm.find("max-concurrent-tasks");

		std::optional<std::string> max_concurrent_tasks_str, max_loaded_targets_size_str;
		if (max_loaded_targets_size_it != vm.end()
			&& !max_loaded_targets_size_it->second.defaulted())
		{
			max_loaded_targets_size_str = string::to_utf8(max_loaded_targets_size_it
				->second.as<std::basic_string<Char>>());
		}
		if (max_concurrent_tasks_it != vm.end())
		{
			max_concurrent_tasks_str = string::to_utf8(max_concurrent_tasks_it
				->second.as<std::basic_string<Char>>());
		}
		
		analysis_plan_parse_helpers::parse_preload_limit(
			max_concurrent_tasks_str,
			max_loaded_targets_size_str,
			plan);
	}

	static void parse_threads(const po::variables_map& vm, analysis_plan& plan)
	{
		if (auto threads_it = vm.find("threads"); threads_it != vm.end())
		{
			analysis_plan_parse_helpers::parse_thread_count(string::to_utf8(threads_it
				->second.as<std::basic_string<Char>>()), plan);
		}
	}

	static void parse_root_path(const po::variables_map& vm, analysis_plan& plan)
	{
		if (auto root_path_it = vm.find("root-path"); root_path_it != vm.end())
		{
			plan.set_root_path(to_path(
				root_path_it->second.as<std::basic_string<Char>>()));
		}
	}

private:
	static std::filesystem::path to_path(std::string_view str)
	{
		return string::utf8_to<std::filesystem::path::string_type>::convert(str);
	}

#if BOOST_OS_WINDOWS
	static std::filesystem::path to_path(std::wstring_view str)
	{
		return to_path(string::to_utf8(str));
	}
#endif //BOOST_OS_WINDOWS

	template<typename T>
	static auto po_value()
	{
		if constexpr (std::is_same_v<Char, char>)
			return po::value<T>();
		else
			return po::wvalue<T>();
	}

	static auto default_1G_string()
	{
		if constexpr (std::is_same_v<Char, char>)
			return std::string("1G");
		else
			return std::wstring(L"1G");
	}

	static po::options_description get_options_description()
	{
		po::options_description general_opts("General options");
		general_opts.add_options()
			("help,h", "Print options description")
			("config,c", po_value<std::basic_string<Char>>(),
				"Path to external XML configuration file. Can not be used with any other options.")
			("root-path,r", po_value<std::basic_string<Char>>(),
				"Root path. This path will be used as a root for all relative paths specified. "
				"If absent, current directory will be used as a root path.")
			("threads", po_value<std::basic_string<Char>>(), "Analysis thread count. "
				"If absent, hardware core number will be used.")
			("max-loaded-targets-size", po_value<std::basic_string<Char>>()
				->default_value(default_1G_string(), "1G"), "Max loaded targets size. "
				"If specified, the analyzer will preload as many targets to analyze as possible "
				"until their size in memory is less than the value specified."
				"Should have a numeric value together with the suffix (B - bytes, "
				"K - kilobytes, M - megabytes, G - gigabytes), e.g. 2G for 2 gigabytes."
				"\nCan not be used together with --max-concurrent-tasks."
				"\nDefault is 1G.")
			("max-concurrent-tasks", po_value< std::basic_string<Char>>(), "Max concurrent analysis tasks. "
				"If specified, the analyzer will preload at most max-concurrent-tasks targets for analysis."
				"\nCan not be used together with --max-loaded-targets-size.");
		
		po::options_description combined_analysis_opts("Combined analysis options");
		combined_analysis_opts.add_options()
			("do-combined-analysis", po_value<bool>()->default_value(true),
				"Do combined analysis of all specified targets. Default is true.");

		po::options_description target_opts("Targets options");
		target_opts.add_options()
			("target,t", po_value<std::vector<std::basic_string<Char>>>(),
				"Target files or directories to analyze. Required.")
			("recursive", po_value<bool>()->default_value(true),
				"Scan and analyze the target directory recursively. Default is true.")
			("include-regex", po_value<std::basic_string<Char>>(),
				"Analyze only targets with full paths which match the provided ECMAScript regex.")
			("exclude-regex", po_value<std::basic_string<Char>>(),
				"Analyze only targets with full paths which do not match the provided ECMAScript regex.");

		po::options_description rule_opts("Report options");
		rule_opts.add_options()
			("exclude-reports", po_value<std::vector<std::basic_string<Char>>>(),
				"Exclude reports with the IDs listed. Can not be used together with --include-reports.")
			("include-reports", po_value<std::vector<std::basic_string<Char>>>(),
				"Include only reports with the IDs listed. Can not be used together with --exclude-reports.")
			("list-reports", "List all supported reports with brief descriptions.")
			("exclude-levels", po_value<std::vector<std::basic_string<Char>>>(),
				"Exclude report levels (info, warning, error, critical).")
			("exclude-categories", po_value<std::vector<std::basic_string<Char>>>(),
				"Exclude report categories (system, optimization, security, configuration, format).")
			("with-progress", "Report verbose analysis progress");

		po::options_description report_opts("Output options");
		report_opts.add_options()
			("silent", "Do not output analysis results to terminal.")
			("sarif,S", po_value<std::basic_string<Char>>(),
				"Path to save the output report in SARIF format.")
			("text,T", po_value<std::basic_string<Char>>(),
				"Path to save the output report in plaintext.");

		po::options_description all_options;
		all_options
			.add(general_opts)
			.add(combined_analysis_opts)
			.add(target_opts)
			.add(rule_opts)
			.add(report_opts);
		return all_options;
	}

private:
	po::options_description opts_;
	int argc_;
	const Char* const* argv_;
};
} //namespace

struct command_line_analysis_plan_provider::impl
{
#if BOOST_OS_WINDOWS
	std::variant<options_helper<char>, options_helper<wchar_t>> opts;
#else //BOOST_OS_WINDOWS
	std::variant<options_helper<char>> opts;
#endif //BOOST_OS_WINDOWS
};

command_line_analysis_plan_provider::command_line_analysis_plan_provider(
	int argc, const char* const* argv,
	const core::rule_list& rules,
	const core::combined_rule_list& combined_rules,
	const string::resource_provider_interface& resources,
	config_file_analysis_plan_provider_type&& config_file_analysis_plan_provider)
	: impl_(std::make_shared<impl>(options_helper(argc, argv)))
	, config_file_analysis_plan_provider_(std::move(config_file_analysis_plan_provider))
	, rules_(rules)
	, combined_rules_(combined_rules)
	, resources_(resources)
{
}

#if BOOST_OS_WINDOWS
command_line_analysis_plan_provider::command_line_analysis_plan_provider(
	int argc, const wchar_t* const* argv,
	const core::rule_list& rules,
	const core::combined_rule_list& combined_rules,
	const string::resource_provider_interface& resources,
	config_file_analysis_plan_provider_type&& config_file_analysis_plan_provider)
	: impl_(std::make_shared<impl>(options_helper(argc, argv)))
	, config_file_analysis_plan_provider_(std::move(config_file_analysis_plan_provider))
	, rules_(rules)
	, combined_rules_(combined_rules)
	, resources_(resources)
{
}
#endif //BOOST_OS_WINDOWS

void command_line_analysis_plan_provider::print_help(std::ostream& stream) const
{
	std::visit([&stream](const auto& opts) {
		stream << opts.get_description();
	}, impl_->opts);
}

namespace
{
template<typename Reports>
std::vector<output::rule_report_base> get_sorted_reports(Reports&& reports)
{
	std::vector<output::rule_report_base> sorted_reports;
	sorted_reports.reserve(reports.size());
	for (const auto& report : reports)
		sorted_reports.emplace_back(report);
	std::ranges::sort(sorted_reports, [](const auto& l, const auto& r) {
		return l.get_string_uid() < r.get_string_uid();
	});
	return sorted_reports;
}

void print_formatted_report_info(std::ostream& stream,
	std::string_view string_uid, std::string_view category,
	std::string_view level, std::string_view description)
{
	static constexpr std::size_t total_length = 7u + 15u + 10u;
	static constexpr std::size_t max_length = 80u;
	stream << std::format("{: <7}{: <15}{: <10}",
		string_uid, category, level);
	std::size_t current_size = total_length;
	for (const auto& word : std::views::split(description, ' '))
	{
		const std::string_view view(word.begin(), word.end());
		current_size += view.size();
		if (current_size > max_length)
		{
			stream << '\n' << std::setfill(' ') << std::setw(total_length) << ' ';
			current_size = total_length + view.size();
		}
		++current_size; //for space
		stream << view << ' ';
	}
	stream << '\n';
}

template<typename Reports>
void print_formatted_reports(std::ostream& stream, const Reports& reports,
	const string::resource_provider_interface& resources)
{
	print_formatted_report_info(stream, "ID",
		"Category", "Level", "Brief description");
	for (const auto& report : reports)
	{
		string::rule_report_resource_helper helper(report, resources);
		print_formatted_report_info(stream,
			report.get_string_uid(),
			helper.get_report_category(),
			helper.get_report_level(),
			helper.get_report_uid_name());
	}
}
} //namespace

void command_line_analysis_plan_provider::print_reports(std::ostream& stream) const
{
	stream << "Supported per-target rules:\n\n";
	print_formatted_reports(stream,
		get_sorted_reports(rules_.get_reports()), resources_);

	stream << "\nSupported combined rules:\n\n";
	print_formatted_reports(stream,
		get_sorted_reports(combined_rules_.get_reports()), resources_);
}

void command_line_analysis_plan_provider::print_header(std::ostream& stream)
{
	stream << version::tool_name << ' '
		<< version::tool_version[0] << '.'
		<< version::tool_version[1] << '.'
		<< version::tool_version[2] << '.'
		<< version::tool_version[3] << '\n';
	stream << version::tool_description << " (" << version::tool_website
		<< ")\n\n";
}

analysis_plan command_line_analysis_plan_provider::get() const
{
	return std::visit([this](const auto& opts) {
		return opts.get_plan(config_file_analysis_plan_provider_,
		rules_, combined_rules_, resources_);
	}, impl_->opts);
}

} //namespace bv::analysis
