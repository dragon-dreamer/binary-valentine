#include "binary_valentine/string/rule_report_resource_helper.h"

#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/string/resource_helper.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::string
{

std::string_view rule_report_resource_helper::get_report_category() const
{
	return string::resource_helper::to_string(
		report_.get_report_category(), resource_provider_);
}

std::string_view rule_report_resource_helper::get_report_level() const
{
	return string::resource_helper::to_string(
		report_.get_report_level(), resource_provider_);
}

std::string_view rule_report_resource_helper::get_report_uid_name() const
{
	return resource_provider_.get_string(report_.get_string_uid());
}

std::string_view rule_report_resource_helper::get_report_description() const
{
	return resource_provider_.get_string(
		report_.get_description_key());
}

std::string_view rule_report_resource_helper::get_rule_name() const
{
	return resource_provider_.get_string(
		report_.get_rule_name_key());
}

} //namespace bv::string
