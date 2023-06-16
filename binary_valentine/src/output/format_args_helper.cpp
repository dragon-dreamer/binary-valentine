#include "binary_valentine/output/format_args_helper.h"

#include <cassert>
#include <cstddef>
#include <exception>
#include <string>
#include <variant>

#define FMT_HEADER_ONLY 1
#include "fmt/args.h"
#include "fmt/core.h"

#include "binary_valentine/output/exception_formatter.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/string/encoding.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::output
{

namespace
{
struct string_report : public common_report_interface
{
	explicit string_report(const string::resource_provider_interface& resource_provider,
		const exception_formatter& formatter) noexcept
		: resource_provider(resource_provider)
		, formatter(formatter)
	{
	}

	virtual void log_impl(report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override
	{
		str = format_args_helper::format_utf8_message(level,
			resource_provider.get_string(message_id),
			args, arg_names, resource_provider, formatter);
	}

	const string::resource_provider_interface& resource_provider;
	const exception_formatter& formatter;
	std::string str;
};
} //namespace

std::string format_args_helper::format_utf8_message(
	report_level level,
	std::string_view message,
	std::span<const arg_type> args,
	std::span<const char* const> arg_names,
	const string::resource_provider_interface& resource_provider,
	const exception_formatter& formatter)
{
	assert(args.size() == arg_names.size());

	fmt::dynamic_format_arg_store<fmt::format_context> store;
	store.reserve(args.size(), args.size());
	string_report exception_report(resource_provider, formatter);
	for (std::size_t i = 0; i != args.size(); ++i)
	{
		const char* arg_name = arg_names[i];
		std::visit([&store, &formatter, level, &exception_report, arg_name,
			&resource_provider](const auto& arg) {
			using nested_type = std::remove_cvref_t<decltype(arg)>;
			if constexpr (std::is_same_v<nested_type, std::exception_ptr>)
			{
				formatter.format(level, arg, exception_report);
				store.push_back(fmt::arg(arg_name, std::move(exception_report.str)));
			}
			else if constexpr (std::is_same_v<nested_type, std::u16string_view>
				|| std::is_same_v<nested_type, std::u32string_view>
				|| std::is_same_v<nested_type, std::wstring_view>
				|| std::is_same_v<nested_type, std::u16string>
				|| std::is_same_v<nested_type, std::u32string>
				|| std::is_same_v<nested_type, std::wstring>)
			{
				std::string utf8_string;
				try
				{
					utf8_string = string::to_utf8(arg);
				}
				catch (const std::exception&)
				{
					utf8_string.assign(1u, '?');
				}

				store.push_back(fmt::arg(arg_name, std::move(utf8_string)));
			}
			else if constexpr (std::is_same_v<nested_type, impl::localizable_string_id>
				|| std::is_same_v<nested_type, impl::owning_localizable_string_id>)
			{
				const auto localized_message = resource_provider.get_string(arg.string_id);

				if (!arg.args.empty())
				{
					fmt::dynamic_format_arg_store<fmt::format_context> temp_store;
					for (const auto& [name, value] : arg.args)
					{
						temp_store.push_back(fmt::arg(name.c_str(),
							string::replace_invalid_characters_copy(value)));
					}

					auto str = fmt::vformat(fmt::string_view(
						localized_message.data(), localized_message.size()),
						temp_store);

					store.push_back(fmt::arg(arg_name, std::move(str)));
				}
				else
				{
					store.push_back(fmt::arg(arg_name, localized_message));
				}
			}
			else if constexpr (std::is_same_v<nested_type, std::string>
				|| std::is_same_v<nested_type, std::string_view>)
			{
				store.push_back(fmt::arg(arg_name,
					string::replace_invalid_characters_copy(arg)));
			}
			else
			{
				store.push_back(fmt::arg(arg_name, arg));
			}
		}, args[i]);
	}
	
	return fmt::vformat(fmt::string_view(message.data(), message.size()), store);
}

void format_args_helper::format_as_string(const arg_type& arg,
	const exception_formatter& formatter,
	arg_value_type& value)
{
	std::visit([&formatter, &value](const auto& arg) {
		using nested_type = std::remove_cvref_t<decltype(arg)>;
		if constexpr (std::is_same_v<nested_type, std::exception_ptr>)
		{
			formatter.format(arg, value);
		}
		else if constexpr (std::is_same_v<nested_type, std::u16string_view>
			|| std::is_same_v<nested_type, std::u32string_view>
			|| std::is_same_v<nested_type, std::wstring_view>
			|| std::is_same_v<nested_type, std::u16string>
			|| std::is_same_v<nested_type, std::u32string>
			|| std::is_same_v<nested_type, std::wstring>)
		{
			std::string utf8_string;
			try
			{
				utf8_string = string::to_utf8(arg);
			}
			catch (const std::exception&)
			{
				utf8_string.assign(1u, '?');
			}

			value = std::move(utf8_string);
		}
		else if constexpr (std::is_same_v<nested_type, impl::localizable_string_id>
			|| std::is_same_v<nested_type, impl::owning_localizable_string_id>)
		{
			if (arg.args.empty())
			{
				value = std::string(arg.string_id);
			}
			else
			{
				auto& vector = value.emplace<multiple_value_arg_type>();
				vector.reserve(1u + arg.args.size());
				vector.emplace_back(arg::string_id, arg.string_id);
				for (const auto& [name, value] : arg.args)
				{
					vector.emplace_back(name,
						string::replace_invalid_characters_copy(value));
				}
			}
		}
		else if constexpr (std::is_same_v<nested_type, std::string>
			|| std::is_same_v<nested_type, std::string_view>)
		{
			value = string::replace_invalid_characters_copy(arg);
		}
		else
		{
			value = fmt::format("{}", arg);
		}
	}, arg);
}

void format_args_helper::format_as_string(
	std::span<const arg_type> args,
	std::span<const char* const> arg_names,
	const exception_formatter& formatter,
	arg_name_value_list_type& values)
{
	values.reserve(args.size());
	for (std::size_t i = 0; i != args.size(); ++i)
	{
		auto& name_value = values.emplace_back(arg_names[i],
			std::in_place_index<0>);
		format_as_string(args[i], formatter, name_value.second);
	}
}

} //namespace bv::output
