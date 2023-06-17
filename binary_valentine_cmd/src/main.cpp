#include <chrono>
#include <iostream>
#include <optional>
#include <string_view>
#include <utility>

#include <boost/predef/os.h>
#include <boost/program_options/errors.hpp>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/analysis/analysis_plan_runner.h"
#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/analysis/command_line_analysis_plan_provider.h"
#include "binary_valentine/analysis/xml_analysis_plan_provider.h"
#include "binary_valentine/core/localizable_error.h"
#include "binary_valentine/core/user_error.h"
#include "binary_valentine/output/exception_formatter.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/terminal_report_output.h"
#include "binary_valentine/string/embedded_resource_loader.h"

namespace
{
struct analysis_return_value final
{
	enum value
	{
		no_issues = 0,
		entity_issues_found = 1,
		analysis_issues_exist = 2,
		entity_and_analysis_issues_exist = 3,
		invalid_options = 4,
		analysis_interrupted = 5,
		internal_error = -1
	};
};

void print_exception(bv::output::terminal_report_output& terminal)
{
	terminal.log(bv::output::report_level::error,
		"{exception}", bv::output::current_exception_arg());
}
} //namespace

#if BOOST_OS_WINDOWS
int wmain(int argc, wchar_t* argv[]) try
#else //BOOST_OS_WINDOWS
int main(int argc, char* argv[]) try
#endif //BOOST_OS_WINDOWS
{
	const bv::analysis::immutable_context context;
	static constexpr std::string_view language("en");
	const auto resources = bv::string::embedded_resource_loader{}.load(language);

	bv::output::terminal_report_output terminal(nullptr,
		*resources, context.get_exception_formatter());

	const bv::analysis::command_line_analysis_plan_provider plan_provider(argc, argv,
		context.get_rules(), context.get_combined_rules(), *resources,
		[&context](std::filesystem::path&& p) {
		return std::make_unique<bv::analysis::xml_analysis_plan_provider>(
			std::move(p), context);
	});

	plan_provider.print_header(std::cout);

	std::optional<bv::analysis::analysis_plan> plan;
	try
	{
		plan = plan_provider.get();
	}
	catch (const boost::program_options::error&)
	{
		print_exception(terminal);
		return analysis_return_value::invalid_options;
	}
	catch (const bv::core::localizable_error& e)
	{
		if (e.code() == bv::core::user_errc::help_requested)
		{
			plan_provider.print_help(std::cout);
			return analysis_return_value::no_issues;
		}
		
		if (e.code() == bv::core::user_errc::reports_info_requested)
		{
			plan_provider.print_reports(std::cout);
			return analysis_return_value::no_issues;
		}

		print_exception(terminal);
		return analysis_return_value::invalid_options;
	}

	bv::analysis::analysis_plan_runner runner(
		std::move(*plan), context, *resources);
	runner.start();
	runner.join();
	const auto state = runner.write_reports();

	terminal.log(bv::output::report_level::info,
		state.interrupted
			? bv::output::reports::analysis_interrupted_message
			: bv::output::reports::analysis_finished_message,
		bv::output::named_arg("duration",
			std::chrono::round<std::chrono::seconds>(state.duration).count()),
		bv::output::named_arg("entity_issues", state.entity_issues),
		bv::output::named_arg("analysis_issues",
			state.analysis_issues - state.interrupted));

	if (state.interrupted)
		return analysis_return_value::analysis_interrupted;
	if (state.entity_issues && state.analysis_issues)
		return analysis_return_value::entity_and_analysis_issues_exist;
	if (state.entity_issues)
		return analysis_return_value::entity_issues_found;
	if (state.analysis_issues)
		return analysis_return_value::analysis_issues_exist;
	return analysis_return_value::no_issues;
}
catch (const std::exception& e)
{
	std::cerr << "Internal error: " << e.what() << '\n';
	return analysis_return_value::internal_error;
}
