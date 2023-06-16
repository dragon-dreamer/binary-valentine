#pragma once

#include <vector>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::output
{

class [[nodiscard]] unformatted_common_output_report final
	: public common_report_interface
{
public:
	virtual void log_impl(report_level level,
		std::string_view message_id,
		std::span<const arg_type> args,
		std::span<const char* const> arg_names) override;

public:
	[[nodiscard]]
	bool empty() const noexcept
	{
		return common_reports_.empty();
	}

	void reyield(common_report_interface& report_output) const;

private:
	struct saved_common_report
	{
		report_level level;
		std::string_view message_id;
		std::vector<arg_type> args;
		std::vector<const char*> arg_names;
	};

	std::vector<saved_common_report> common_reports_;
};

} //namespace bv::output
