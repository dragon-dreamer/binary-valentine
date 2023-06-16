#pragma once

#include <filesystem>
#include <functional>
#include <iosfwd>
#include <memory>

#include <boost/predef/os.h>

#include "binary_valentine/analysis/analysis_plan_provider_interface.h"
#include "binary_valentine/core/rule_list.h"

namespace bv::string
{
class resource_provider_interface;
} //namespace bv::string

namespace bv::analysis
{

class [[nodiscard]] command_line_analysis_plan_provider final
	: public analysis_plan_provider_interface
{
public:
	using config_file_analysis_plan_provider_type =
		std::function<std::unique_ptr<analysis_plan_provider_interface>(
			std::filesystem::path&&)>;
public:
	explicit command_line_analysis_plan_provider(int argc, const char* const* argv,
		const core::rule_list& rules,
		const core::combined_rule_list& combined_rules,
		const string::resource_provider_interface& resources,
		config_file_analysis_plan_provider_type&& config_file_analysis_plan_provider);

#ifdef BOOST_OS_WINDOWS
	explicit command_line_analysis_plan_provider(int argc, const wchar_t* const* argv,
		const core::rule_list& rules,
		const core::combined_rule_list& combined_rules,
		const string::resource_provider_interface& resources,
		config_file_analysis_plan_provider_type&& config_file_analysis_plan_provider);
#endif //BOOST_OS_WINDOWS

	[[nodiscard]]
	virtual analysis_plan get() const override;

	void print_help(std::ostream& stream) const;

	static void print_header(std::ostream& stream);
	
	void print_reports(std::ostream& stream) const;

private:
	struct impl;
	std::shared_ptr<impl> impl_;
	config_file_analysis_plan_provider_type config_file_analysis_plan_provider_;
	const core::rule_list& rules_;
	const core::combined_rule_list& combined_rules_;
	const string::resource_provider_interface& resources_;
};

} //namespace bv::analysis
