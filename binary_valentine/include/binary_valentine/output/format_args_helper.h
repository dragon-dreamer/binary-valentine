#pragma once

#include <span>
#include <string>
#include <string_view>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::string { class resource_provider_interface; }

namespace bv::output
{

class exception_formatter;

class format_args_helper final
{
public:
	format_args_helper() = default;

	[[nodiscard]]
	static std::string format_utf8_message(
		report_level level,
		std::string_view message,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names,
		const string::resource_provider_interface& resource_provider,
		const exception_formatter& formatter);

	static void format_as_string(const arg_type& arg,
		const exception_formatter& formatter,
		arg_value_type& value);

	static void format_as_string(
		std::span<const arg_type> args,
		std::span<const char* const> arg_names,
		const exception_formatter& formatter,
		arg_name_value_list_type& values);
};

} //namespace bv::output
