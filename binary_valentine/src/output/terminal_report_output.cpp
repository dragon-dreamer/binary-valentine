#include "binary_valentine/output/terminal_report_output.h"

#include <exception>
#include <format>
#include <iostream>
#include <syncstream>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/string/resource_helper.h"
#include "binary_valentine/string/resource_provider_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"
#include "binary_valentine/output/format_args_helper.h"

namespace bv::output
{

void terminal_report_output::output_entity_name(std::osyncstream& stream) const
{
	if (entity_)
	{
		auto name = entity_->get_name();
		if (!name.empty())
			stream << name << ": ";
	}
}

void terminal_report_output::log_impl(report_level level,
	std::string_view message_id,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	std::osyncstream stream(std::cout);
	output_entity_name(stream);

	stream << '(' << string::resource_helper::to_string(level, resource_provider_)
		<< ") "
		<< format_args_helper::format_utf8_message(
			level,
			resource_provider_.get_string(message_id),
			args,
			arg_names,
			resource_provider_,
			formatter_)
		<< '\n';
}

void terminal_report_output::rule_log_impl(const rule_report_base& info,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names)
{
	std::osyncstream stream(std::cout);
	output_entity_name(stream);

	string::rule_report_resource_helper helper(info, resource_provider_);

	stream << info.get_string_uid()
		<< " (" << helper.get_report_level()
		<< ", " << helper.get_report_category()
		<< ") "
		<< helper.get_report_uid_name()
		<< "\n  "
		<< format_args_helper::format_utf8_message(
			info.get_report_level(),
			helper.get_report_description(),
			args,
			arg_names,
			resource_provider_, formatter_)
		<< '\n';
}

} //namespace bv::output
