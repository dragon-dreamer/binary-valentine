#include "binary_valentine/string/resource_helper.h"

#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::string
{

class resource_provider;

std::string_view resource_helper::to_string(
	output::report_category cat,
	const resource_provider_interface& provider)
{
	using enum output::report_category;
	switch (cat)
	{
	case system: return provider.get_string("report_category_system");
	case optimization: return provider.get_string("report_category_optimization");
	case security: return provider.get_string("report_category_security");
	case configuration: return provider.get_string("report_category_configuration");
	case format: return provider.get_string("report_category_format");
	default: return provider.get_string("report_category_unknown");
	}
}

std::string_view resource_helper::to_string(
	output::report_level cat,
	const resource_provider_interface& provider)
{
	using enum output::report_level;
	switch (cat)
	{
	case info: return provider.get_string("report_level_info");
	case warning: return provider.get_string("report_level_warning");
	case error: return provider.get_string("report_level_error");
	case critical: return provider.get_string("report_level_critical");
	default: return provider.get_string("report_level_unknown");
	}
}

} //namespace bv::string
