#include "binary_valentine/output/exception_formatter.h"

#include <array>
#include <exception>
#include <format>
#include <system_error>

#include "binary_valentine/core/localizable_error.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/string/encoding.h"

namespace bv::output
{

void exception_formatter::format(
	report_level level,
	std::exception_ptr exception,
	common_report_interface& report) const
{
	for (const auto& extra : extra_formatters_)
	{
		if (extra->try_format(level, exception, report))
			return;
	}

	try
	{
		std::rethrow_exception(exception);
	}
	catch (const core::localizable_error& e)
	{
		report.log_noexcept(level,
			reports::user_error_description,
			localizable_arg(arg::error_message, { e.get_error_string_id(), e.get_args() }));
	}
	catch (const std::system_error& e)
	{
		if (e.code().category() == std::system_category())
		{
			const std::error_condition condition =
				std::system_category().default_error_condition(e.code().value());

			report.log_noexcept(level,
				reports::system_error_description,
				named_arg(arg::error_category_name, std::string_view(e.code().category().name())),
				named_arg(arg::error_code_value, e.code().value()),
				named_arg(arg::error_message, condition.message()));

			return;
		}

		report.log_noexcept(level,
			reports::system_error_description,
			named_arg(arg::error_category_name, std::string_view(e.code().category().name())),
			named_arg(arg::error_code_value, e.code().value()),
			named_arg(arg::error_message, e.code().message()));
	}
	catch (const std::exception& e)
	{
		report.log_noexcept(level,
			reports::exception_description,
			named_arg(arg::error_message, std::string(e.what())));
	}
}

void exception_formatter::format(std::exception_ptr exception,
	arg_value_type& value) const
{
	for (const auto& extra : extra_formatters_)
	{
		if (extra->try_format(exception, value))
			return;
	}

	try
	{
		std::rethrow_exception(exception);
	}
	catch (const core::localizable_error& e)
	{
		if (e.get_args().empty())
		{
			value = std::string(e.get_error_string_id());
		}
		else
		{
			auto& vector = value.emplace<multiple_value_arg_type>();
			vector.reserve(1u + e.get_args().size());
			vector.emplace_back(arg::string_id, e.get_error_string_id());
			for (const auto& [name, value] : e.get_args())
			{
				vector.emplace_back(name,
					string::replace_invalid_characters_copy(value));
			}
		}
	}
	catch (const std::system_error& e)
	{
		auto& vector = value.emplace<multiple_value_arg_type>();
		vector.emplace_back(arg::error_category_name,
			e.code().category().name());
		vector.emplace_back(arg::error_code_value,
			std::to_string(e.code().value()));

		if (e.code().category() == std::system_category())
		{
			const std::error_condition condition =
				std::system_category().default_error_condition(e.code().value());
			vector.emplace_back(arg::error_message,
				condition.message());
		}
		else
		{
			vector.emplace_back(arg::error_message,
				e.code().message());
		}
	}
	catch (const std::exception& e)
	{
		value = e.what();
	}
}

} //namespace bv::output
