#pragma once

#include <array>
#include <cstddef>
#include <chrono>
#include <concepts>
#include <format>
#include <list>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <boost/pfr/core.hpp>

#include "binary_valentine/core/compile_time_string.h"

namespace bv::json
{

template<typename Key, typename T>
struct generic_field
{
public:
	template<typename Value>
	void operator=(Value&& val)
	{
		value = std::forward<Value>(val);
	}

	[[nodiscard]]
	T* operator->() noexcept
	{
		return &value;
	}

	[[nodiscard]]
	const T* operator->() const noexcept
	{
		return &value;
	}

public:
	Key key;
	T value;
};

template<typename Key, typename T, typename Formatter>
struct custom_field : generic_field<Key, T>
{
	template<typename Value>
	void operator=(Value&& val)
	{
		this->value = std::forward<Value>(val);
	}
};

template<typename Field>
struct custom_field_set
{
	std::vector<Field> fields;
};

template<typename T>
using field = generic_field<std::string_view, T>;

template<typename T>
using optional_field = generic_field<std::string_view, std::optional<T>>;

using string_field = field<std::string_view>;
using boolean_field = field<bool>;
using owning_string_field = field<std::string>;
using optional_string_field = optional_field<std::string_view>;

template<typename T, std::size_t Size>
using array_field = field<std::array<T, Size>>;
template<typename T, std::size_t Size>
using optional_array_field = optional_field<std::array<T, Size>>;

template<typename T>
using vector_field = field<std::vector<T>>;
template<typename T>
using optional_vector_field = optional_field<std::vector<T>>;

template<typename T>
using list_field = field<std::list<T>>;
template<typename T>
using optional_list_field = optional_field<std::list<T>>;

namespace impl
{
template<typename T>
struct is_optional : std::false_type {};
template<typename T>
struct is_optional<std::optional<T>> : std::true_type {};

struct output_options
{
	std::size_t indent;
	std::string_view indent_string;
};

inline void do_indentation(const output_options& opts, std::ostream& out)
{
	for (std::size_t i = 0; i != opts.indent; ++i)
		out << opts.indent_string;
}

inline void print_escaped_char(char ch, std::ostream& out)
{
	switch (ch)
	{
	case '"': out << "\\\""; break;
	case '\\': out << "\\\\"; break;
	case '\b': out << "\\b"; break;
	case '\f': out << "\\f"; break;
	case '\n': out << "\\n"; break;
	case '\r': out << "\\r"; break;
	case '\t': out << "\\t"; break;
	default: out << ch; break;
	}
}

inline void print_escaped_string(std::string_view value, std::ostream& out)
{
	for (char ch : value)
		print_escaped_char(ch, out);
}

inline void print_value(const output_options&,
	std::string_view value, std::ostream& out)
{
	out << '"';
	print_escaped_string(value, out);
	out << '"';
}

template<std::integral T>
requires (!std::is_same_v<T, bool>)
void print_value(const output_options&,
	const T& value, std::ostream& out)
{
	out << value;
}

inline void print_value(const output_options&,
	bool value, std::ostream& out)
{
	out << (value ? "true" : "false");
}

template<typename... T>
void print_value(const output_options& opts,
	const std::variant<T...>& value, std::ostream& out)
{
	std::visit([&opts, &out] (const auto& nested) {
		print_value(opts, nested, out);
	}, value);
}

template<typename Field>
void print_value(const output_options& opts,
	const custom_field_set<Field>& set, std::ostream& out)
{
	auto count = set.fields.size();
	for (const auto& field : set.fields)
	{
		print_value(opts, field, out);
		if (--count)
			out << ',' << '\n';
	}
}

template<typename Key, typename T, typename Formatter>
void print_value(const output_options& opts,
	const custom_field<Key, T, Formatter>& f, std::ostream& out)
{
	if constexpr (is_optional<T>::value)
	{
		if (!f.value.has_value())
			return;
	}

	do_indentation(opts, out);
	out << '"';
	print_escaped_string(f.key, out);
	out << "\": ";
	if constexpr (is_optional<T>::value)
		Formatter{}(opts, *f.value, out);
	else
		Formatter{}(opts, f.value, out);
}

template<typename Key, typename T>
void print_value(const output_options& opts,
	const generic_field<Key, T>& f, std::ostream& out)
{
	if constexpr (is_optional<T>::value)
	{
		if (!f.value.has_value())
			return;
	}

	do_indentation(opts, out);
	out << '"';
	print_escaped_string(f.key, out);
	out << "\": ";
	if constexpr (is_optional<T>::value)
		print_value(opts, *f.value, out);
	else
		print_value(opts, f.value, out);
}

template<typename Field>
constexpr bool has_value(const Field&)
{
	return true;
}

template<typename Key, typename T>
constexpr bool has_value(const generic_field<Key, T>& f)
{
	if constexpr (is_optional<T>::value)
		return f.value.has_value();
	else
		return true;
}

template<typename Key, typename T, typename F>
constexpr bool has_value(const custom_field<Key, T, F>& f)
{
	if constexpr (is_optional<T>::value)
		return f.value.has_value();
	else
		return true;
}

template<typename Field>
constexpr bool has_value(const custom_field_set<Field>& f)
{
	return !f.fields.empty();
}

template<typename T>
requires (std::is_class_v<T>)
void print_value(const output_options& opts,
	const T& value, std::ostream& out)
{
	std::size_t value_count{};
	boost::pfr::for_each_field(value, [&value_count](const auto& nested) {
		value_count += has_value(nested);
	});

	out << '{';
	if (value_count)
	{
		out << '\n';
		output_options nested_options{ opts };
		++nested_options.indent;
		boost::pfr::for_each_field(value,
			[&nested_options, &out, &value_count](const auto& nested) {
			if (!value_count || !has_value(nested))
				return;
			print_value(nested_options, nested, out);
			if (--value_count)
				out << ',';
			out << '\n';
		});
		do_indentation(opts, out);
	}
	out << '}';
}

template<typename Arr>
void print_array(const output_options& opts,
	const Arr& value, std::ostream& out)
{
	out << '\n';
	output_options nested_options{ opts };
	++nested_options.indent;
	std::size_t remaining{ value.size()};
	for (const auto& nested : value)
	{
		do_indentation(nested_options, out);
		print_value(nested_options, nested, out);
		if (--remaining)
			out << ',';
		out << '\n';
	}
	do_indentation(opts, out);
}

template<typename T, std::size_t Size>
void print_value(const output_options& opts,
	const std::array<T, Size>& value, std::ostream& out)
{
	out << '[';
	if constexpr (Size)
		print_array(opts, value, out);
	out << ']';
}

template<typename T>
void print_value(const output_options& opts,
	const std::vector<T>& value, std::ostream& out)
{
	out << '[';
	if (!value.empty())
		print_array(opts, value, out);
	out << ']';
}

template<typename T>
void print_value(const output_options& opts,
	const std::list<T>& value, std::ostream& out)
{
	out << '[';
	if (!value.empty())
		print_array(opts, value, out);
	out << ']';
}

struct empty_char_printer
{
	constexpr bool operator()(char, std::ostream&) noexcept
	{
		return false;
	}
};

} //namespace impl

struct utc_time_formatter final
{
	template<typename TimePoint>
	void operator()(const impl::output_options&,
		const TimePoint& time_point, std::ostream& out) const
	{
		auto utc_time_point = std::chrono::clock_cast<
			std::chrono::utc_clock>(time_point);
		out << std::format("\"{:%Y-%m-%dT%T}Z\"", utc_time_point);
	}
};

template<typename CustomCharPrinter = impl::empty_char_printer>
struct uri_formatter final
{
	template<typename Str>
	void operator()(const impl::output_options&,
		const Str& str, std::ostream& out) const
	{
		CustomCharPrinter printer;

		out << "\"file:///";

		out << std::hex << std::uppercase;
		for (char ch : str)
		{
			if (printer(ch, out))
				continue;

			if ((ch >= 'a' && ch <= 'z')
				|| (ch >= 'A' && ch <= 'Z')
				|| (ch >= '0' && ch <= '9')
				|| ch == '.' || ch == '-'
				|| ch == '~' || ch == '_')
			{
				out << ch;
				continue;
			}

			out << '%';
			out << static_cast<std::uint32_t>(
				static_cast<std::uint8_t>(ch));
		}

		out << '"';
		out << std::dec << std::nouppercase;
	}
};

template<typename Json>
void print_json(const Json& root, std::ostream& out)
{
	impl::output_options opts{ 0, "    " };
	impl::print_value(opts, root, out);
}

} //namespace bv::json
