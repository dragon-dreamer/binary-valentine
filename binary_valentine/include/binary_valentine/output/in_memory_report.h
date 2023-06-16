#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::output
{

struct [[nodiscard]] saved_rule_report
{
	rule_report_base report;
	std::string formatted_message;
	arg_name_value_list_type arg_name_to_arg;
};

struct [[nodiscard]] saved_common_report
{
	report_level level{};
	std::string_view message_id;
	std::string formatted_message;
	arg_name_value_list_type arg_name_to_arg;
};

using saved_rule_reports_type = std::vector<saved_rule_report>;
using saved_common_reports_type = std::vector<saved_common_report>;

} //namespace bv::output
