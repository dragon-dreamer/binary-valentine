#include "binary_valentine/output/format/html_report_output_format.h"

#include <array>
#include <format>
#include <string_view>
#include <stdexcept>

#include "binary_valentine/analysis/result_report_file.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/core/user_error.h"
#include "binary_valentine/file/file_loader.h"
#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/string/resource_helper.h"
#include "binary_valentine/string/resource_provider_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/version.h"

#include "inja/inja.hpp"

#define FMT_HEADER_ONLY 1
#include "fmt/args.h"
#include "fmt/core.h"

namespace bv::output::format
{

struct html_report_output_format::impl
{
public:
	static constexpr std::string_view default_html_template_file_name = "html_report.tpl";

	impl(const analysis::result_report_file::options_map_type& extra_options,
		const core::embedded_resource_loader_interface& resource_loader,
		const string::resource_provider_interface& resource_provider)
		: resource_provider_(resource_provider)
	{
		std::string_view template_file_name = default_html_template_file_name;
		try
		{
			if (auto it = extra_options.find(
				analysis::result_report_file::report_template_extra_argument_name);
				it != extra_options.cend())
			{
				template_file_name = it->second;
				template_ = file::file_loader::load_file(std::string_view(it->second));
			}
			else
			{
				template_ = resource_loader
					.load_file(resource_provider_.get_string(default_html_template_file_name));
			}
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(
				core::user_error(core::user_errc::unable_to_load_html_report_template_file,
					core::user_error::arg_type("file", std::string(template_file_name))));
		}
	}

	static std::string htmlescape(std::string_view data, std::string_view style = {})
	{
		std::string buffer;
		if (!style.empty())
		{
			buffer.reserve(static_cast<std::size_t>(
				1.2 * data.size() + style.size()));
			buffer.append("<span class='");
			buffer.append(style);
			buffer.append("'>");
		}
		else
		{
			buffer.reserve(static_cast<std::size_t>(1.1 * data.size()));
		}

		for (auto ch : data)
		{
			switch (ch)
			{
			case '&': buffer.append("&amp;"); break;
			case '\"': buffer.append("&quot;"); break;
			case '\'': buffer.append("&apos;"); break;
			case '<': buffer.append("&lt;"); break;
			case '>': buffer.append("&gt;"); break;
			default: buffer += ch; break;
			}
		}
		if (!style.empty())
			buffer.append("</span>");
		return buffer;
	}

	std::string format_message(std::string_view message_id,
		std::string_view formatted_message_fallback,
		const arg_name_value_list_type& arg_name_to_arg) const
	{
		const std::string format_str = htmlescape(resource_provider_.get_string(message_id));
		try
		{
			fmt::dynamic_format_arg_store<fmt::format_context> store;
			store.reserve(arg_name_to_arg.size(), arg_name_to_arg.size());
			for (const auto& [arg_name, arg_value] : arg_name_to_arg)
			{
				if (const auto* arg_str = std::get_if<std::string>(&arg_value); arg_str)
				{
					store.push_back(fmt::arg(arg_name.data(), htmlescape(*arg_str, "formatted-arg")));
					continue;
				}

				const auto& arg_vec = std::get<multiple_value_arg_type>(arg_value);
				if (arg_vec.empty())
					return htmlescape(formatted_message_fallback);

				const auto& string_id = arg_vec.front();
				const bool is_localizable_string = string_id.first == arg::localizable_string_id;
				if (!is_localizable_string && string_id.first != arg::string_id)
					return htmlescape(formatted_message_fallback);

				std::string_view nested_format_str = resource_provider_.get_string(string_id.second);
				std::string localized_format_str;
				fmt::dynamic_format_arg_store<fmt::format_context> nested_store;
				nested_store.reserve(arg_vec.size() - 1u, arg_vec.size() - 1u);
				auto args_start = arg_vec.begin() + 1;
				if (is_localizable_string)
				{
					const std::string_view nested_string = resource_provider_.get_string(
						arg_vec.at(1).second);
					nested_store.push_back(fmt::arg(arg_vec.at(1).first.c_str(), nested_string));
					localized_format_str = fmt::vformat(nested_format_str, nested_store);
					nested_format_str = localized_format_str;
					nested_store.clear();
					++args_start;
				}

				for (auto it = args_start; it != arg_vec.end(); ++it)
					nested_store.push_back(fmt::arg(it->first.c_str(), it->second));

				store.push_back(fmt::arg(arg_name.data(),
					htmlescape(fmt::vformat(nested_format_str, nested_store), "formatted-arg")));
			}

			return fmt::vformat(format_str, store);
		}
		catch (const fmt::format_error&)
		{
			return htmlescape(formatted_message_fallback);
		}
	}

	void add_common_reports(const entity_in_memory_report_interface& report,
		std::vector<nlohmann::json>& result)
	{
		for (const auto& common_report : report.get_common_reports())
		{
			nlohmann::json& common_report_json = result.emplace_back();
			common_report_json["level"] = static_cast<std::uint32_t>(common_report.level);
			common_report_json["message"] = format_message(common_report.message_id,
				common_report.formatted_message,
				common_report.arg_name_to_arg);
		}
	}

	static bool has_format(const core::rule_class_mask& detected_rule_types,
		bv::rule_class_type format)
	{
		return detected_rule_types.is_set(format);
	}

	static std::string_view get_file_format(
		const core::rule_class_mask& detected_rule_types)
	{
		if (has_format(detected_rule_types, bv::rule_class_type::pe32))
			return "PE32";

		if (has_format(detected_rule_types, bv::rule_class_type::pe64))
			return "PE64";

		return {};
	}

	void add(const analysis_state& state,
		const extended_analysis_state& extra_state)
	{
		template_args_["tool_name"] = htmlescape(version::tool_name);
		template_args_["tool_website"] = htmlescape(version::tool_website);
		template_args_["rule_info_uri"] = htmlescape(version::rule_info_uri);
		template_args_["tool_version"] = std::format("{0}.{1}.{2}",
			version::tool_version[0], version::tool_version[1], version::tool_version[2]);
		template_args_["github_url"] = htmlescape(version::github_url);

		template_args_["total_analysis_issues"] = state.analysis_issues;
		const auto total_minutes = std::chrono::floor<std::chrono::minutes>(state.duration);
		template_args_["analysis_duration_min"] = total_minutes.count();
		template_args_["analysis_duration_sec"] = std::chrono::ceil<std::chrono::seconds>(
			state.duration - total_minutes).count();
		template_args_["total_entity_issues"] = state.entity_issues;
		template_args_["was_interrupted"] = state.interrupted;
		const auto utc_time_start = std::chrono::clock_cast<
			std::chrono::utc_clock>(state.start);
		template_args_["analysis_start_time"]
			= std::format("{:%Y-%m-%d %H:%M} UTC", utc_time_start);

		template_args_["total_analyzed"] = extra_state.total_analyzed;
		template_args_["total_skipped_filtered"] = extra_state.total_skipped_filtered;
		template_args_["total_skipped_unsupported"] = extra_state.total_skipped_unsupported;
	}

	void add(const entity_in_memory_report_interface& report)
	{
		const auto& entity = report.get_entity();
		if (!entity)
		{
			add_common_reports(report, common_issues_);
			return;
		}
		
		auto& entity_json = entities_.emplace_back();
		entity_json["location"] = htmlescape(entity->get_utf8_path());
		entity_json["name"] = htmlescape(entity->get_name());
		entity_json["index"] = index_++;
		if (entity->is_regular_file())
		{
			entity_json["is_file"] = true;
			entity_json["file_size"] = entity->get_size();
			const auto utc_time_last_modified = std::chrono::clock_cast<
				std::chrono::utc_clock>(entity->get_last_write_time());
			entity_json["last_modified"]
				= std::format("{:%Y-%m-%d %H:%M} UTC", utc_time_last_modified);
			entity_json["file_format"]
				= get_file_format(report.get_detected_rule_types());
		}
		else
		{
			entity_json["is_file"] = false;
		}

		std::vector<nlohmann::json> common_reports;
		add_common_reports(report, common_reports);
		entity_json["common_reports"] = std::move(common_reports);

		std::vector<nlohmann::json> rule_reports;
		for (const auto& rule_report : report.get_rule_reports())
		{
			auto& result = rule_reports.emplace_back();
			result["rule_id"] = rule_report.report.get_string_uid();
			result["level"] = rule_report.report.get_report_level();

			const string::rule_report_resource_helper helper(
				rule_report.report, resource_provider_);
			result["name"] = helper.get_report_uid_name();
			result["category"] = helper.get_report_category();
			++issues_per_category_[static_cast<std::size_t>(
				rule_report.report.get_report_category())];
			result["category_id"] = static_cast<std::uint32_t>(
				rule_report.report.get_report_category());

			result["message"] = format_message(
				rule_report.report.get_description_key(),
				rule_report.formatted_message,
				rule_report.arg_name_to_arg);
		}

		// Sort by severity in reverse order
		std::sort(rule_reports.begin(), rule_reports.end(),
			[](const nlohmann::json& l, const nlohmann::json& r) {
				return l.at("level") > r.at("level");
			});
		entity_json["rule_reports"] = std::move(rule_reports);
	}

	void finalize(const std::filesystem::path& path)
	{
		try
		{
			std::ofstream out;
			out.exceptions(std::ios::badbit | std::ios::failbit);
			out.open(path, std::ios::out | std::ios::binary | std::ios::trunc);

			inja::Environment env;
			env.set_search_included_templates_in_files(false);
			env.set_html_autoescape(false);

			// Sort by severity in reverse order
			std::sort(common_issues_.begin(), common_issues_.end(),
				[](const nlohmann::json& l, const nlohmann::json& r) {
					return l.at("level") > r.at("level");
				});

			std::sort(entities_.begin(), entities_.end(),
				[](const nlohmann::json& l, const nlohmann::json& r) {
				return l.at("name") < r.at("name");
			});

			template_args_["entities"] = std::move(entities_);
			template_args_["common_issues"] = std::move(common_issues_);

			template_args_["issues_per_category"] = issues_per_category_;

			env.render_to(out, template_, template_args_);
		}
		catch (const std::exception& e)
		{
			std::throw_with_nested(
				core::user_error(core::user_errc::unable_to_write_html_report,
					core::user_error::arg_type("underlying", e.what())));
		}
	}

private:
	const string::resource_provider_interface& resource_provider_;
	std::string template_;
	nlohmann::json template_args_;
	std::vector<nlohmann::json> entities_;
	std::vector<nlohmann::json> common_issues_;
	std::size_t index_{};
	std::array<std::size_t, static_cast<std::size_t>(report_category::max)> issues_per_category_{};
};

html_report_output_format::html_report_output_format(
	const string::resource_provider_interface& resource_provider,
	std::filesystem::path&& path,
	const analysis::result_report_file::options_map_type& extra_options,
	const core::embedded_resource_loader_interface& resource_loader)
	: path_(std::move(path))
	, impl_(std::make_unique<impl>(extra_options, resource_loader, resource_provider))
{
}

html_report_output_format::~html_report_output_format() = default;

void html_report_output_format::start(const analysis_state& state,
	const std::optional<extended_analysis_state>& extra_state)
{
	impl_->add(state, extra_state.value());
}

void html_report_output_format::append(const entity_in_memory_report_interface& report)
{
	impl_->add(report);
}

void html_report_output_format::finalize()
{
	impl_->finalize(path_);
}

} //namespace bv::output::format
