#pragma once

#include <array>
#include <cstddef>
#include <exception>
#include <memory>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <boost/predef/os/windows.h>

#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/rule_report.h"

namespace bv::core
{
class subject_entity_interface;
class rule_selector;
} //namespace bv::core

namespace bv::output
{

class exception_formatter;

template<typename Arg>
struct named_arg
{
	named_arg(const char* name, std::remove_cvref_t<Arg>&& val)
		requires(!std::is_reference_v<Arg>)
		: name(name)
		, arg(std::move(val))
	{
	}

	named_arg(const char* name, const std::remove_cvref_t<Arg>& val)
		requires(std::is_lvalue_reference_v<Arg>)
		: name(name)
		, arg(std::move(val))
	{
	}

	const char* const name;
	Arg arg;
};

template<typename Str, typename Arg>
named_arg(Str, Arg&&) -> named_arg<std::remove_cvref_t<Arg>>;
template<typename Str, typename Arg>
named_arg(Str, Arg&) -> named_arg<const std::remove_cvref_t<Arg>&>;
template<typename Str, typename Arg>
named_arg(Str, const Arg&) -> named_arg<const std::remove_cvref_t<Arg>&>;

struct current_exception_arg
{
	const char* const name{ arg::exception };
	std::exception_ptr arg{ std::current_exception() };
};

namespace impl
{
struct localizable_string_id
{
	constexpr localizable_string_id(std::string_view string_id) noexcept
		: string_id(string_id)
	{
	}

	constexpr localizable_string_id(std::string_view string_id,
		std::span<const std::pair<std::string, std::string>> args) noexcept
		: string_id(string_id)
		, args(args)
	{
	}

	std::string_view string_id;
	std::span<const std::pair<std::string, std::string>> args;
};

struct owning_localizable_string_id
{
	explicit owning_localizable_string_id(const localizable_string_id& id)
		: string_id(id.string_id)
		, args(id.args.begin(), id.args.end())
	{
	}

	std::string string_id;
	std::vector<std::pair<std::string, std::string>> args;
};
} //namespace impl

using localizable_arg = named_arg<impl::localizable_string_id>;

using arg_type = std::variant<
	bool,
	std::int32_t,
	std::uint32_t,
	std::int64_t,
	std::uint64_t,
	float,
	double,
	std::string_view,
	std::u16string_view,
	std::u32string_view,
	std::string,
	std::u16string,
	std::u32string,
#if BOOST_OS_WINDOWS
	std::wstring_view,
	std::wstring,
#endif //BOOST_OS_WINDOWS
	std::exception_ptr,
	impl::localizable_string_id,
	impl::owning_localizable_string_id
>;

class [[nodiscard]] common_report_interface
{
public:
	virtual ~common_report_interface() = default;

	template<typename... NamedArgs>
	void log(report_level level,
		std::string_view message_id,
		NamedArgs&&... named_args)
	{
		std::array<arg_type, sizeof...(NamedArgs)> args{
			std::forward<NamedArgs>(named_args).arg... };
		std::array<const char*, sizeof...(NamedArgs)> arg_names{
			named_args.name... };
		log_impl(level, message_id, args, arg_names);
	}

	template<typename... NamedArgs>
	void log_noexcept(report_level level,
		std::string_view message_id,
		const NamedArgs&... named_args) noexcept
	{
		try
		{
			log(level, message_id, named_args...);
		}
		catch (...)
		{
		}
	}

protected:
	virtual void log_impl(report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) = 0;

	static void forward_log(common_report_interface& target,
		report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names)
	{
		target.log_impl(level, message_id, args, arg_names);
	}
};

class [[nodiscard]] entity_report_interface : public common_report_interface
{
public:
	template<typename... NamedArgs>
	void rule_log(const rule_report_base& info,
		NamedArgs&&... named_args)
	{
		std::array<arg_type, sizeof...(NamedArgs)> args{
			std::forward<NamedArgs>(named_args).arg... };
		std::array<const char*, sizeof...(NamedArgs)> arg_names{
			named_args.name... };
		rule_log_impl(info, args, arg_names);
	}

	template<typename... NamedArgs>
	void rule_log_noexcept(const rule_report_base& info,
		const NamedArgs&... named_args) noexcept
	{
		try
		{
			rule_log(info, named_args...);
		}
		catch (...)
		{
		}
	}

protected:
	virtual void rule_log_impl(const rule_report_base& info,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) = 0;

	void forward_rule_log(entity_report_interface& target,
		const rule_report_base& info,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names)
	{
		target.rule_log_impl(info, args, arg_names);
	}
};

class [[nodiscard]] result_report_factory_interface
{
public:
	virtual ~result_report_factory_interface() = default;

	[[nodiscard]]
	virtual std::shared_ptr<entity_report_interface> get_entity_report(
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		const core::rule_selector& selector,
		const exception_formatter& formatter) = 0;

	[[nodiscard]]
	virtual std::shared_ptr<common_report_interface> get_common_report(
		const core::rule_selector& selector,
		const exception_formatter& formatter) = 0;
};

using multiple_value_arg_type = std::vector<std::pair<std::string, std::string>>;
using arg_value_type = std::variant<std::string, multiple_value_arg_type>;
using arg_name_value_list_type = std::vector<std::pair<std::string_view, arg_value_type>>;

} //namespace bv::output
