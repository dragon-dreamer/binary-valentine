#include "binary_valentine/analysis/xml_analysis_plan_provider.h"

#include <charconv>
#include <concepts>
#include <exception>
#include <system_error>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/analysis_plan_parse_helpers.h"
#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/core/overloaded.h"
#include "binary_valentine/core/user_error.h"
#include "binary_valentine/common/xml_loader.h"

namespace bv::analysis
{

namespace
{

void parse_root_path(const pugi::xml_node& plan_root, analysis_plan& plan)
{
	const auto& root_path = plan_root.attribute("root_path");
	if (root_path)
		analysis_plan_parse_helpers::parse_root_path(root_path.value(), plan);
}

void parse_thread_count(const pugi::xml_node& plan_root, analysis_plan& plan)
{
	const auto& thread_count = plan_root.attribute("thread_count");
	if (thread_count)
		analysis_plan_parse_helpers::parse_thread_count(thread_count.value(), plan);
}

void parse_preload_limit(const pugi::xml_node& plan_root, analysis_plan& plan)
{
	const auto& max_concurrent_tasks_attr = plan_root.attribute("max_concurrent_tasks");
	const auto& max_loaded_targets_size_attr = plan_root.attribute("max_loaded_targets_size");
	analysis_plan_parse_helpers::parse_preload_limit(
		max_concurrent_tasks_attr
			? max_concurrent_tasks_attr.value() : std::optional<std::string_view>{},
		max_loaded_targets_size_attr
			? max_loaded_targets_size_attr.value() : std::optional<std::string_view>{},
		plan);
}

void parse_combined_analysis_options(const pugi::xml_node& plan_root, analysis_plan& plan)
{
	const auto& combined_analysis = plan_root.attribute("combined_analysis");
	if (!combined_analysis)
		return;

	analysis_plan_parse_helpers::parse_combined_analysis_option(
		combined_analysis.value(), plan);
}

void parse_report_list(const pugi::xml_node& root,
	const immutable_context& context, core::rule_selector& selector)
{
	for (const auto& nested : root)
	{
		if (std::string_view("report") != nested.name())
		{
			throw core::user_error(core::user_errc::unsupported_report_list_tag,
				core::user_error::arg_type("tag", nested.name()));
		}

		analysis_plan_parse_helpers::add_selected_report(
			nested.text().as_string(), context.get_rules(),
			context.get_combined_rules(), selector);
	}
}

void parse_excluded_levels(const pugi::xml_node& root,
	core::rule_selector& selector)
{
	for (const auto& nested : root)
	{
		if (std::string_view("level") != nested.name())
		{
			throw core::user_error(core::user_errc::unsupported_excluded_levels_list_tag,
				core::user_error::arg_type("tag", nested.name()));
		}

		analysis_plan_parse_helpers::add_excluded_level(
			nested.text().as_string(), selector);
	}
}

void parse_excluded_categories(const pugi::xml_node& root,
	core::rule_selector& selector)
{
	for (const auto& nested : root)
	{
		if (std::string_view("category") != nested.name())
		{
			throw core::user_error(core::user_errc::unsupported_excluded_categories_list_tag,
				core::user_error::arg_type("tag", nested.name()));
		}

		analysis_plan_parse_helpers::add_excluded_category(
			nested.text().as_string(), selector);
	}
}

void parse_report_filter(const pugi::xml_node& root,
	const immutable_context& context, core::rule_selector& selector)
{
	const auto& report_attr = root.attribute("report");
	if (!report_attr)
		throw core::user_error(core::user_errc::absent_report_filter_report_attribute);

	auto [report_filter, created] = selector.get_or_create_report_selector(
		analysis_plan_parse_helpers::get_report_uid(report_attr.value(),
			context.get_rules(), context.get_combined_rules()));
	if (!created)
	{
		throw core::user_error(core::user_errc::duplicate_report_filter,
			core::user_error::arg_type("filter", report_attr.value()));
	}

	if (const auto& selection_mode_attr = root.attribute("selection_mode");
		selection_mode_attr)
	{
		report_filter.get().set_selection_mode(
			analysis_plan_parse_helpers::parse_selection_mode(selection_mode_attr.value()));
	}

	if (const auto& aggregation_mode_attr = root.attribute("aggregation_mode");
		aggregation_mode_attr)
	{
		report_filter.get().set_aggregation_mode(
			analysis_plan_parse_helpers::parse_aggregation_mode(aggregation_mode_attr.value()));
	}

	for (const auto& nested : root)
	{
		if (std::string_view("regex") != nested.name())
		{
			throw core::user_error(core::user_errc::unsupported_report_filter_tag,
				core::user_error::arg_type("tag", nested.name()));
		}

		const auto& arg_name_attr = nested.attribute("arg");
		if (!arg_name_attr)
			throw core::user_error(core::user_errc::absent_report_filter_arg);

		analysis_plan_parse_helpers::parse_report_filter_regex(
			arg_name_attr.value(), nested.text().as_string(), report_filter);
	}
}

std::optional<pugi::xml_node> get_single_node(const pugi::xml_node& root_node,
	const char* tag_name)
{
	const auto& nodes = root_node.children(tag_name);
	if (nodes.empty())
		return {};

	if (std::next(nodes.begin()) != nodes.end())
	{
		throw core::user_error(core::user_errc::multiple_xml_elements,
			core::user_error::arg_type("tag", tag_name));
	}

	return *nodes.begin();
}

void parse_rule_selector(const pugi::xml_node& root_node,
	const char* selector_tag_name,
	const immutable_context& context, core::rule_selector& selector)
{
	auto selector_node = get_single_node(root_node, selector_tag_name);
	if (!selector_node)
		return;

	bool has_selected_rules = false;
	bool has_excluded_levels = false;
	bool has_excluded_categories = false;
	for (const auto& nested : *selector_node)
	{
		std::string_view name(nested.name());
		if (name == "include_reports" || name == "exclude_reports")
		{
			if (has_selected_rules)
				throw core::user_error(core::user_errc::duplicate_included_excluded_reports);
			has_selected_rules = true;
			selector.set_mode(name == "exclude_reports"
				? core::rule_selection_mode::exclude_selected
				: core::rule_selection_mode::include_selected);
			parse_report_list(nested, context, selector);
			continue;
		}

		if (name == "exclude_levels")
		{
			if (has_excluded_levels)
				throw core::user_error(core::user_errc::duplicate_excluded_levels);
			has_excluded_levels = true;
			parse_excluded_levels(nested, selector);
			continue;
		}

		if (name == "exclude_categories")
		{
			if (has_excluded_categories)
				throw core::user_error(core::user_errc::duplicate_excluded_categories);
			has_excluded_categories = true;
			parse_excluded_categories(nested, selector);
			continue;
		}

		if (name == "report_filter")
		{
			parse_report_filter(nested, context, selector);
			continue;
		}

		throw core::user_error(core::user_errc::unsupported_rule_selector_tag,
			core::user_error::arg_type("tag", std::string(name)));
	}
}

void parse_global_selector(const pugi::xml_node& plan_root,
	const immutable_context& context, analysis_plan& plan)
{
	parse_rule_selector(plan_root, "global_selector",
		context, plan.get_global_rule_selector());
}

void parse_target_filters(const pugi::xml_node& target_node, plan_target& target)
{
	auto filter_node = get_single_node(target_node, "filter");
	if (!filter_node)
		return;

	for (const auto& nested : *filter_node)
	{
		std::string_view node_name(nested.name());
		std::string_view regex(nested.text().as_string());
		if (node_name == "include")
		{
			analysis_plan_parse_helpers::parse_target_filter_regex(
				regex, true, target.get_target_filter());
		}
		else if (node_name == "exclude")
		{
			analysis_plan_parse_helpers::parse_target_filter_regex(
				regex, false, target.get_target_filter());
		}
		else
		{
			throw core::user_error(core::user_errc::unsupported_filter_tag,
				core::user_error::arg_type("tag", std::string(node_name)));
		}
	}
}

void parse_targets(const pugi::xml_node& plan_root,
	const immutable_context& context, analysis_plan& plan)
{
	auto targets_node = get_single_node(plan_root, "targets");
	if (!targets_node)
		throw core::user_error(core::user_errc::no_targets_specified);

	for (const auto& target_node : *targets_node)
	{
		if (target_node.name() != std::string_view("target"))
		{
			throw core::user_error(core::user_errc::unsupported_targets_tag,
				core::user_error::arg_type("tag", target_node.name()));
		}

		const auto& path_attr = target_node.attribute("path");
		std::string_view target_path(path_attr.value());
		if (!path_attr || target_path.empty())
			throw core::user_error(core::user_errc::absent_target_path);

		auto& target = plan.emplace_target(target_path);
		if (const auto& recursive_attr = target_node.attribute("recursive"); recursive_attr)
		{
			target.set_recursive(analysis_plan_parse_helpers::to_bool<
				core::user_errc::invalid_recursive_attribute_value>(recursive_attr.value()));
		}

		if (!target_node.children("rule_selector").empty())
		{
			parse_rule_selector(target_node, "rule_selector",
				context, target.emplace_rule_selector());
		}

		parse_target_filters(target_node, target);
	}

	if (plan.get_targets().empty())
		throw core::user_error(core::user_errc::no_targets_specified);
}

void parse_file_output(const pugi::xml_node& report_node,
	result_report_file_type type, analysis_plan& plan)
{
	std::string_view path(report_node.text().as_string());
	if (path.empty())
		throw core::user_error(core::user_errc::empty_report_path);

	plan.emplace_output_report(result_report_file(path, type));
}

void parse_output_reports(const pugi::xml_node& plan_root, analysis_plan& plan)
{
	auto reports_node = get_single_node(plan_root, "reports");
	if (!reports_node)
		throw core::user_error(core::user_errc::no_reports_specified);

	bool has_terminal_output = false;
	for (const auto& report_node : *reports_node)
	{
		std::string_view node_name(report_node.name());

		if (node_name == "terminal")
		{
			if (!has_terminal_output)
			{
				has_terminal_output = true;
				plan.emplace_output_report(result_report_terminal{});
			}
			continue;
		}

		if (node_name == "text")
		{
			parse_file_output(report_node, result_report_file_type::text, plan);
			continue;
		}

		if (node_name == "sarif")
		{
			parse_file_output(report_node, result_report_file_type::sarif, plan);
			continue;
		}

		throw core::user_error(core::user_errc::unsupported_reports_tag,
			core::user_error::arg_type("tag", std::string(node_name)));
	}

	if (plan.get_result_reports().empty())
		throw core::user_error(core::user_errc::no_reports_specified);
}

} //namespace

/*
<plan root_path="C:/" thread_count="3" max_loaded_targets_size="1G" combined_analysis="true">
	<global_selector>
		<exclude_reports>
			<report>PE001</report>
			<report>PE002</report>
		</exclude_reports>
		<exclude_levels>
			<level>info</level>
		</exclude_levels>
		<exclude_categories>
			<category>format</category>
		</exclude_categories>
		<report_filter report="PE003" selection_mode="exclude" aggregation_mode="any">
			<regex arg="alala">some_regex</regex>
		</report_filter>
	</global_selector>

	<targets>
		<target recursive="false" path="file.exe">
			<rule_selector>
				<exclude_reports>
					<report>PE001</report>
				</exclude_reports>
			</rule_selector>
			<filter>
				<include>regex</include>
				<exclude>regex2</exclude>
			</filter>
		</target>
	</targets>

	<reports>
		<terminal />
		<text>c:/report.txt</text>
		<sarif>c:/report.sarif.json</sarif>
	</reports>
</plan>
*/

analysis_plan xml_analysis_plan_provider::get() const
{
	pugi::xml_document config;

	try
	{
		config = std::visit(core::overloaded{
			[](const std::string& str) {
				return common::xml_loader::load_xml(std::string_view(str));
			},
			[](const std::filesystem::path& path) {
				return common::xml_loader::load_xml(path);
			},
		}, xml_);
	}
	catch (const std::runtime_error& e)
	{
		throw core::user_error(core::user_errc::unable_to_load_xml,
			core::user_error::arg_type("error", e.what()));
	}

	analysis_plan result;

	const auto& plan_root = config.child("plan");
	parse_root_path(plan_root, result);
	parse_thread_count(plan_root, result);
	parse_preload_limit(plan_root, result);
	parse_combined_analysis_options(plan_root, result);
	parse_global_selector(plan_root, context_, result);
	parse_targets(plan_root, context_, result);
	parse_output_reports(plan_root, result);

	return result;
}

} //namespace bv::analysis
