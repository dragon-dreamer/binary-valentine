#pragma once

#include <memory>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/output/format/output_format_interface.h"

namespace bv::output
{
class common_report_interface;
} //namespace bv::output

namespace bv::string
{
class resource_provider_interface;
} //namespace bv::string

namespace bv::analysis
{

class immutable_context;

class [[nodiscard]] analysis_plan_runner
{
public:
	explicit analysis_plan_runner(analysis_plan&& plan,
		const immutable_context& global_context,
		const string::resource_provider_interface& resources);
	analysis_plan_runner(const analysis_plan_runner&) = delete;
	analysis_plan_runner& operator=(const analysis_plan_runner&) = delete;
	analysis_plan_runner(analysis_plan_runner&&) = default;
	analysis_plan_runner& operator=(analysis_plan_runner&&) = default;

	void start();

	void join();

	output::format::analysis_state write_reports(
		output::common_report_interface* error_log);

	void interrupt();

private:
	struct impl;

private:
	std::shared_ptr<impl> impl_;
};

} //namespace bv::analysis
