#pragma once

#include "binary_valentine/output/rule_report.h"

namespace bv::executable_report
{
//electron_version_rule
using unstable_electron_version = output::rule_report_info<
	"EXE001", output::report_category::security, output::report_level::warning>;
using outdated_electron_version = output::rule_report_info<
	"EXE002", output::report_category::security, output::report_level::error>;
//NEXT: EXE003
} //namespace bv::executable_report
