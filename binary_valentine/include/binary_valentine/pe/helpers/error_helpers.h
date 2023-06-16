#pragma once

#include <cstddef>
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
		typename Reporter, typename Value, typename ErrorMap>
	static void report_errors(Reporter& reporter,
		const Value& value, const ErrorMap& map)
	{
		if (!value.has_errors())
			return;

		for (const auto& [ctx, exception] : *value.get_errors())
		{
			auto error_code_it = map.find(ctx.code);
			if (error_code_it != map.cend())
			{
				reporter.template log_error_code<Report>(ctx.code,
					output::localizable_arg("description",
						error_code_it->second));
			}
			else
			{
				reporter.template log_error_code<Report>(ctx.code,
					output::localizable_arg("description",
						output::reports::description_not_available));
			}
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