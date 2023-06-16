#pragma once

#include "binary_valentine/output/rule_report.h"

namespace bv::string
{

class resource_provider_interface;

class resource_helper final
{
public:
	resource_helper() = delete;

	[[nodiscard]]
	static std::string_view to_string(
		output::report_category cat,
		const resource_provider_interface& provider);

	[[nodiscard]]
	static std::string_view to_string(
		output::report_level cat,
		const resource_provider_interface& provider);
};

} //namespace bv::string
