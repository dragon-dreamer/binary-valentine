#include "binary_valentine/pe/rule/image_errors_rule.h"

#include <string_view>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/error_list.h"

namespace bv::pe
{

class image_errors_rule final
	: public core::rule_base<image_errors_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_image_errors_rule";
	static constexpr auto reports = output::get_rule_reports<
		image_errors_rule,
		pe_report::image_load_error,
		pe_report::image_load_warning>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::error_list* warnings,
		const std::exception_ptr* load_error) const
	{
		if (load_error && *load_error)
		{
			reporter.template log<pe_report::image_load_error>(
				output::named_arg(output::arg::exception, *load_error));
			return;
		}

		if (warnings && warnings->has_errors())
		{
			for (const auto& [ctx, info] : *warnings->get_errors())
			{
				try
				{
					throw std::system_error(ctx.code);
				}
				catch (...)
				{
					reporter.template log<pe_report::image_load_warning>(
						output::current_exception_arg());
				}
			}
		}
	}
};

void image_errors_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<image_errors_rule>(shared_values);
}

} //namespace bv::pe
