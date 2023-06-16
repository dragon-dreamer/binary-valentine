#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

#include "binary_valentine/core/compile_time_string.h"

namespace bv::output
{

enum class report_category
{
	system,
	optimization,
	security,
	configuration,
	format,
	max
};

enum class report_level
{
	info,
	warning,
	error,
	critical,
	max
};

[[nodiscard]]
constexpr bool operator<(report_level l, report_level r) noexcept
{
	return static_cast<std::uint32_t>(l) < static_cast<std::uint32_t>(r);
}

struct report_uid final
{
	bool operator==(const report_uid&) const = default;

	[[nodiscard]]
	constexpr bool is_valid() const noexcept
	{
		return value != nullptr;
	}

	[[nodiscard]]
	constexpr explicit operator bool() const noexcept
	{
		return value != nullptr;
	}

	using id_type = const void*;
	id_type value{};
};

class [[nodiscard]] rule_report_base
{
public:
	constexpr rule_report_base(report_uid uid,
		std::string_view string_uid,
		report_category category,
		report_level level,
		std::string_view description_key) noexcept
		: uid_(uid)
		, string_uid_(string_uid)
		, category_(category)
		, level_(level)
		, description_key_(description_key)
	{
	}

	constexpr rule_report_base() noexcept = default;

	[[nodiscard]]
	constexpr operator report_uid() const noexcept
	{
		return report_uid{ uid_ };
	}

	[[nodiscard]]
	constexpr report_uid get_uid() const noexcept
	{
		return uid_;
	}

	[[nodiscard]]
	constexpr std::string_view get_string_uid() const noexcept
	{
		return string_uid_;
	}

	[[nodiscard]]
	constexpr report_category get_report_category() const noexcept
	{
		return category_;
	}

	[[nodiscard]]
	constexpr report_level get_report_level() const noexcept
	{
		return level_;
	}

	[[nodiscard]]
	constexpr std::string_view get_description_key() const noexcept
	{
		return description_key_;
	}

private:
	report_uid uid_{};
	std::string_view string_uid_;
	report_category category_{};
	report_level level_{};
	std::string_view description_key_;
};

template<core::compile_time_string StringUid,
	report_category Cat, report_level Level>
struct [[nodiscard]] rule_report_info final
{
public:
	rule_report_info() = delete;

	static constexpr core::compile_time_string string_uid = StringUid;
	static constexpr report_category cat = Cat;
	static constexpr report_level level = Level;
	static constexpr core::compile_time_string description_key{
		core::concat<StringUid, "_description">() };
};

template<typename Rule, typename RuleReportInfo>
class [[nodiscard]] rule_report final : public rule_report_base
{
public:
	using rule_report_info_type = RuleReportInfo;

public:
	constexpr rule_report() noexcept
		: rule_report_base(report_uid{ &uid },
			RuleReportInfo::string_uid.str,
			RuleReportInfo::cat,
			RuleReportInfo::level,
			RuleReportInfo::description_key.str)
	{
	}

private:
	static inline int uid{};
};

template<typename Rule, typename... Reports>
[[nodiscard]]
consteval auto get_rule_reports() noexcept
{
	return std::array<output::rule_report_base, sizeof...(Reports)>{
		output::rule_report<Rule, Reports>{}...
	};
}

} //namespace bv::output

template<>
struct std::hash<bv::output::report_uid>
{
	std::size_t operator()(bv::output::report_uid uid) const noexcept
	{
		return std::hash<bv::output::report_uid::id_type>{}(uid.value);
	}
};
