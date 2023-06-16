#include "binary_valentine/output/format/text_output_format.h"

#include <algorithm>
#include <cstddef>
#include <format>
#include <numeric>
#include <vector>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/string/resource_helper.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/version.h"

namespace bv::output::format
{

void text_output_format::start(const analysis_state& state)
{
	out_.exceptions(std::ios::badbit | std::ios::failbit);
	out_.open(path_, std::ios::out | std::ios::binary | std::ios::trunc);
	out_ << version::tool_name << ' '
		<< version::tool_version[0] << '.'
		<< version::tool_version[1] << '.'
		<< version::tool_version[2] << '.'
		<< version::tool_version[3] << '\n'
		<< std::format("{:%Y-%m-%dT%T}Z", state.start)
		<< " - " << std::format("{:%Y-%m-%dT%T}Z", state.end) << '\n' << '\n';
}

void text_output_format::append(const entity_in_memory_report_interface& report)
{
	const char* indent = "";
	if (report.get_entity())
	{
		out_ << report.get_entity()->get_name() << '\n';
		indent = "\t";
	}

	for (const auto& common_report : report.get_common_reports())
	{
		out_ << indent
			<< string::resource_helper::to_string(
				common_report.level, resource_provider_);
		out_ << indent << '\t'
			<< common_report.formatted_message
			<< '\n';
	}

	const auto& rule_reports = report.get_rule_reports();

	//Sort by level in reverse order
	std::vector<std::size_t> indexes(report.get_rule_reports().size());
	std::iota(indexes.begin(), indexes.end(), 0u);
	std::sort(indexes.begin(), indexes.end(), [&rule_reports](
		std::size_t l, std::size_t r) {
		return rule_reports[r].report.get_report_level()
			< rule_reports[l].report.get_report_level();
	});

	for (std::size_t index : indexes)
	{
		string::rule_report_resource_helper helper(
			rule_reports[index].report, resource_provider_);
		out_ << indent
			<< rule_reports[index].report.get_string_uid()
			<< " (" << helper.get_report_level()
			<< ", " << helper.get_report_category()
			<< ") "
			<< helper.get_report_uid_name()
			<< '\n';
		out_ << indent << '\t'
			<< rule_reports[index].formatted_message
			<< '\n';
	}

	out_ << '\n';
}

void text_output_format::finalize()
{
	out_.close();
}

} //namespace bv::output::format
