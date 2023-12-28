#pragma once

#include <cstddef>
#include <system_error>
#include <vector>

#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/output/internal_report_messages.h"

#include "pe_bliss2/error_list.h"

namespace bv::pe
{

class error_helpers final
{
public:
	error_helpers() = delete;

	template<typename Report,
		typename Reporter, typename Value, typename ErrorMap, typename... NamedArgs>
	static void report_errors(Reporter& reporter,
		const Value& value, const ErrorMap& map, const NamedArgs&... args)
	{
		if (!value.has_errors())
			return;

		for (const auto& [ctx, exception] : *value.get_errors())
			report_error<Report>(reporter, ctx.code, map, args...);
	}

	template<typename Report, typename Reporter,
		typename ErrorMap, typename... NamedArgs>
	static void report_error(Reporter& reporter,
		std::error_code ec, const ErrorMap& map, const NamedArgs&... args)
	{
		if (!ec)
			return;

		auto error_code_it = map.find(ec);
		if (error_code_it != map.cend())
		{
			reporter.template log_error_code<Report>(ec,
				output::localizable_arg("description",
					error_code_it->second),
				args...);
		}
		else
		{
			reporter.template log_error_code<Report>(ec,
				output::localizable_arg("description",
					output::reports::description_not_available),
				args...);
		}
	}

	template<typename Report,
		typename Reporter, typename Value, typename ErrorMap>
	static void report_errors(Reporter& reporter,
		const std::vector<Value>& values, const ErrorMap& map)
	{
		pe_bliss::error_list merged;
		merge_errors(values, merged);
		report_errors<Report>(reporter, merged, map);
	}

	template<typename Value>
	static void merge_errors(const std::vector<Value>& values,
		pe_bliss::error_list& merged)
	{
		for (const auto& value : values)
			merge_errors(value, merged);
	}

	template<typename Value>
	static void merge_errors(const Value& value,
		pe_bliss::error_list& merged)
	{
		if (!value.has_errors())
			return;

		for (const auto& [ctx, exception] : *value.get_errors())
			merged.add_error(ctx.code);
	}
};

} //namespace bv::pe