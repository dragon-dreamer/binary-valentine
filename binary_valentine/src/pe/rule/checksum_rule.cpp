#include "binary_valentine/pe/rule/checksum_rule.h"

#include <string_view>
#include <system_error>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/checksum.h"

namespace bv::pe
{

class checksum_rule final
	: public core::rule_base<checksum_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_checksum_rule";
	static constexpr auto reports = output::get_rule_reports<
		checksum_rule,
		pe_report::absent_checksum,
		pe_report::incorrect_checksum,
		pe_report::unable_to_compute_checksum>();

	template<typename Reporter>
	void run(Reporter& reporter, const pe_bliss::image::image& image,
		const basic_pe_info& info) const
	{
		const auto checksum = image.get_optional_header().get_raw_checksum();
		if (!checksum)
		{
			if (info.is_driver || image.get_data_directories().has_security())
				reporter.template log<pe_report::absent_checksum>();
			
			return;
		}

		pe_bliss::image::image_checksum_type expected_checksum{};
		try
		{
			expected_checksum = pe_bliss::image::calculate_checksum(image);
		}
		catch (const std::system_error&)
		{
			reporter.template log<pe_report::unable_to_compute_checksum>(
				output::named_arg("checksum", checksum),
				output::named_arg(output::arg::exception, std::current_exception()));
			return;
		}

		if (checksum != expected_checksum)
		{
			reporter.template log<pe_report::incorrect_checksum>(
				output::named_arg("checksum", checksum),
				output::named_arg("expected_checksum", expected_checksum));
		}
	}
};

void checksum_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<checksum_rule>();
}

} //namespace bv::pe
