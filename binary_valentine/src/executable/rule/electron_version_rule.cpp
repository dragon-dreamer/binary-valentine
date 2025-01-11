#include "binary_valentine/executable/rule/electron_version_rule.h"

#include <format>
#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/executable/executable_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "binary_valentine/executable/data/electron_app_info.h"
#include "binary_valentine/executable/shared_data/recent_electron_version_info.h"

namespace bv::executable
{

class electron_version_rule final
	: public core::rule_base<electron_version_rule>
{
public:
	// pe only for now, but can be elf/mach-o/etc in the future
	static constexpr auto rule_class = std::array{ rule_class_type::pe };
	static constexpr std::string_view rule_name = "electron_version_rule";
	static constexpr auto reports = output::get_rule_reports<
		electron_version_rule,
		executable_report::unstable_electron_version,
		executable_report::outdated_electron_version>();

public:
	template<typename Reporter>
	void run(Reporter& reporter,
		const electron_app_info& app_info,
		const recent_electron_version_info* recent_electron_versions) const
	{
		if (!app_info.electron_version.has_value())
			return;

		if (app_info.electron_version->build_type != electron_build_type::stable)
		{
			reporter.template log<executable_report::unstable_electron_version>(
				output::named_arg("version", app_info.electron_version->full_string));
		}

		if (recent_electron_versions)
		{
			if (*app_info.electron_version < recent_electron_versions->recent_stable_version)
			{
				reporter.template log<executable_report::outdated_electron_version>(
					output::named_arg("version", app_info.electron_version->full_string),
					output::named_arg("recent_version",
						recent_electron_versions->recent_stable_version.full_string));
			}
		}
	}
};

void electron_version_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<electron_version_rule>();
}

} //namespace bv::executable
