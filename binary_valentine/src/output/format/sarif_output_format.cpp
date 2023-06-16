#include "binary_valentine/output/format/sarif_output_format.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <list>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include "binary_valentine/core/compile_time_string.h"
#include "binary_valentine/core/overloaded.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/json/json_printer.h"
#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/string/resource_provider_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/version.h"

namespace bv::output::format
{

struct custom_uri_char_printer
{
	bool operator()(char ch, std::ostream& out)
	{
		if (ch == static_cast<char>(
			std::filesystem::path::preferred_separator))
		{
			ch = '/';
		}

		if (ch == '/' || ch == ':')
		{
			out << ch;
			return true;
		}

		return false;
	}
};

using uri_field = json::custom_field<std::string_view,
	std::string_view, json::uri_formatter<custom_uri_char_printer>>;

struct multiformat_message_string_type
{
	json::string_field text{ "text" };
};

struct message_strings_type
{
	json::field<multiformat_message_string_type> message{ "message" };
};

struct reporting_configuration_type
{
	json::string_field level{ "level" };
};

struct rule_properties_type
{
	json::array_field<std::string_view, 1u> tags{ "tags" };
};

struct rule_type
{
	json::string_field id{ "id" };
	json::field<multiformat_message_string_type> short_description{ "shortDescription" };
	json::field<message_strings_type> message_strings{ "messageStrings" };
	json::field<reporting_configuration_type> default_configuration{ "defaultConfiguration" };
	json::field<rule_properties_type> properties{ "properties" };
};

struct version_formatter
{
	void operator()(const auto&,
		const std::array<unsigned int, 4u>& version, std::ostream& out) const
	{
		out << '"' << version[0] << '.'
			<< version[1] << '.' << version[2] << '.' << version[3] << '"';
	}
};

struct semantic_version_formatter
{
	void operator()(const auto&,
		const std::array<unsigned int, 4u>& version, std::ostream& out) const
	{
		out << '"' << version[0] << '.'
			<< version[1] << '.' << version[2] << '"';
	}
};

struct driver_type
{
	json::string_field name{ "name", version::tool_name };
	json::custom_field<std::string_view,
		decltype(version::tool_version),
		version_formatter> version{ "version", version::tool_version };
	json::custom_field<std::string_view,
		decltype(version::tool_version),
		semantic_version_formatter> semantic_version{
		"semanticVersion", version::tool_version };
	json::field<multiformat_message_string_type> short_description{
		"shortDescription" };
	json::list_field<rule_type> rules { "rules" };
};

struct tool_type
{
	json::field<driver_type> driver{ "driver" };
};

struct artifact_location_type
{
	uri_field uri{ "uri" };
	json::field<std::size_t> index{ "index" };
};

struct physical_location_type
{
	json::field<artifact_location_type> artifact_location{ "artifactLocation" };
};

struct any_location_type
{
	json::optional_field<physical_location_type> physical_location{ "physicalLocation" };
};

struct multi_value_field_type
{
	json::custom_field_set<json::field<std::string_view>> custom_fields;
};

struct property_bag_type
{
	json::custom_field_set<json::field<
		std::variant<std::string_view, multi_value_field_type>>> custom_fields;
};

struct message_details_type
{
	json::field<property_bag_type> arguments{ "arguments" };
	json::field<std::string_view> unformatted_message{ "unformattedMessage" };
};

struct message_type
{
	json::string_field text{ "text" };
	json::optional_field<message_details_type> properties{ "properties" };
};

struct result_type
{
	json::optional_string_field rule_id{ "ruleId" };
	json::field<std::size_t> rule_index{ "ruleIndex" };
	json::string_field level{ "level" };
	json::field<message_type> message{ "message" };
	json::optional_array_field<any_location_type, 1u> locations{ "locations" };
	json::optional_field<property_bag_type> properties{ "properties" };
};

using file_time_type = json::custom_field<std::string_view,
	std::optional<std::filesystem::file_time_type>, json::utc_time_formatter>;
using utc_time_type = json::custom_field<std::string_view,
	std::chrono::utc_clock::time_point, json::utc_time_formatter>;

struct artifact_type
{
	json::field<artifact_location_type> location{ "location" };
	json::optional_field<std::uint64_t> length{ "length" };
	file_time_type last_modified{ "lastModifiedTimeUtc" };
};

struct reporting_descriptor_reference_type
{
	json::string_field id{ "id" };
	json::optional_field<std::size_t> index{ "index" };
};

struct notification_type
{
	json::optional_field<reporting_descriptor_reference_type>
		associated_rule{ "associatedRule" };
	json::string_field level{ "level" };
	json::field<message_type> message{ "message" };
	json::optional_array_field<any_location_type, 1u> locations{ "locations" };
};

struct invocations_type
{
	utc_time_type start_time_utc{ "startTimeUtc" };
	utc_time_type end_time_utc{ "endTimeUtc" };
	json::boolean_field execution_successful{ "executionSuccessful" };
	json::optional_vector_field<notification_type> execution_notifications{
		"toolExecutionNotifications" };
};

struct run_type
{
	json::field<tool_type> tool{ "tool" };
	json::vector_field<result_type> results{ "results" };
	json::optional_vector_field<artifact_type> artifacts{ "artifacts" };
	json::array_field<invocations_type, 1u> invocations{ "invocations" };
	json::string_field language{ "language" };
};

struct sarif_log_type
{
	json::string_field version{ "version", "2.1.0" };
	json::string_field schema{ "$schema",
		"https://docs.oasis-open.org/sarif/sarif/v2.1.0/cos02/schemas/sarif-schema-2.1.0.json" };
	json::array_field<run_type, 1u> runs{ "runs" };
};

namespace
{
std::string_view get_category_name(report_category category)
{
	switch (category)
	{
	case report_category::configuration:
		return "configuration";
	case report_category::format:
		return "format";
	case report_category::optimization:
		return "optimization";
	case report_category::security:
		return "security";
	case report_category::system:
		return "system";
	default:
		return "unknown";
	}
}

std::string_view get_report_level_name(report_level level)
{
	switch (level)
	{
	case report_level::info:
		return "note";
		break;
	case report_level::warning:
		return "warning";
		break;
	default: //error, critical
		return "error";
		break;
	}
}

template<typename Report>
void add_message_arguments(const Report& report,
	const string::resource_provider_interface& resource_provider,
	message_type& message)
{
	if (!report.arg_name_to_arg.empty())
	{
		auto& custom_fields = message.properties->emplace();
		auto& arguments = custom_fields.arguments->custom_fields.fields;
		for (const auto& [key, value] : report.arg_name_to_arg)
		{
			std::visit(core::overloaded{
				[&arguments, &key](const std::string& value) {
					arguments.emplace_back(key, value);
				},
				[&arguments, &key](const multiple_value_arg_type& value) {
					multi_value_field_type multi_field;
					for (const auto& [nested_key, nested_value] : value)
					{
						multi_field.custom_fields.fields.emplace_back(
							nested_key, nested_value);
					}
					arguments.emplace_back(key, std::move(multi_field));
				}
			}, value);
		}

		if constexpr (std::is_same_v<Report, saved_rule_report>)
		{
			string::rule_report_resource_helper helper(
				report.report, resource_provider);
			custom_fields.unformatted_message = helper.get_report_description();
		}
		else
		{
			custom_fields.unformatted_message
				= resource_provider.get_string(report.message_id);
		}
	}
}
} //namespace

struct rule_info
{
	const rule_type& rule;
	std::size_t index;
};

struct sarif_output_format::impl
{
	sarif_log_type log;

	result_type& add_result()
	{
		return log.runs->front().results->emplace_back();
	}

	artifact_type& add_artifact()
	{
		auto& artifacts = log.runs->front().artifacts;
		if (!artifacts->has_value())
			artifacts->emplace();

		return artifacts->value().emplace_back();
	}

	notification_type& add_execution_notification()
	{
		auto& execution_notifications = log.runs->front()
			.invocations->front().execution_notifications;
		if (!execution_notifications->has_value())
			execution_notifications->emplace();

		return execution_notifications->value().emplace_back();
	}

	template<typename T>
	void add_location(T& result, std::string_view location)
	{
		auto& artifact_location = result.locations.value.emplace()[0]
			.physical_location.value.emplace().artifact_location;
		artifact_location->uri = location;
		artifact_location->index
			= log.runs->front().artifacts->value().size() - 1u;
	}

	const rule_info& add_or_get_rule(const rule_report_base& report,
		const string::resource_provider_interface& resources)
	{
		if (auto it = rules_.find(report.get_uid()); it != rules_.cend())
			return it->second;

		auto& rules = log.runs->front().tool->driver->rules;
		auto& rule = rules->emplace_back();
		
		string::rule_report_resource_helper helper(report, resources);
		rule.id = report.get_string_uid();
		rule.short_description->text = helper.get_report_uid_name();
		rule.message_strings->message->text = helper.get_report_description();
		rule.properties->tags->front() = get_category_name(
			report.get_report_category());
		rule.default_configuration->level = get_report_level_name(
			report.get_report_level());

		return rules_.emplace(report.get_uid(),
			rule_info{ rule, rules->size() - 1u }).first->second;
	}

private:
	std::unordered_map<report_uid, rule_info> rules_;
};

void sarif_output_format::start(const analysis_state& state)
{
	impl_ = std::make_shared<impl>();
	auto& run = impl_->log.runs->front();
	run.tool->driver->short_description->text = version::tool_description;
	run.language = resource_provider_.get_language();
	auto& invocation = run.invocations->front();
	invocation.start_time_utc = state.start;
	invocation.end_time_utc = state.end;
	invocation.execution_successful = !state.interrupted;
}

void sarif_output_format::append(const entity_in_memory_report_interface& report)
{
	std::string_view location;
	const auto* entity = report.get_entity();
	if (entity)
	{
		location = entity->get_utf8_path();

		auto& artifact = impl_->add_artifact();
		if (entity->is_regular_file())
		{
			artifact.length = entity->get_size();
			artifact.last_modified = entity->get_last_write_time();
		}
		artifact.location->uri = location;
	}

	if (!entity || !report.get_common_reports().empty())
	{
		impl_->log.runs->front()
			.invocations->front().execution_successful = false;
	}

	for (const auto& common_report : report.get_common_reports())
	{
		auto& notification = impl_->add_execution_notification();
		notification.level = get_report_level_name(common_report.level);
		notification.message->text = common_report.formatted_message;
		add_message_arguments(common_report,
			resource_provider_, notification.message.value);

		if (entity)
			impl_->add_location(notification, location);
	}

	for (const auto& rule_report : report.get_rule_reports())
	{
		auto& result = impl_->add_result();
		result.rule_id = rule_report.report.get_string_uid();
		result.message->text = rule_report.formatted_message;
		result.level = get_report_level_name(
			rule_report.report.get_report_level());
		result.properties->emplace().custom_fields.fields.emplace_back("category",
			get_category_name(rule_report.report.get_report_category()));

		result.rule_index = impl_->add_or_get_rule(
			rule_report.report, resource_provider_).index;

		add_message_arguments(rule_report, resource_provider_, result.message.value);

		if (!location.empty())
			impl_->add_location(result, location);
	}
}

void sarif_output_format::finalize()
{
	std::ofstream out;
	out.exceptions(std::ios::badbit | std::ios::failbit);
	out.open(path_, std::ios::out | std::ios::binary | std::ios::trunc);
	json::print_json(impl_->log, out);
}

} //namespace bv::output::format
