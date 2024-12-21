#include "binary_valentine/pe/rule/security_cookie_rule.h"

#include <cstddef>
#include <string_view>
#include <type_traits>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/image/image.h"
#include "pe_bliss2/debug/debug_directory.h"
#include "pe_bliss2/load_config/load_config_directory.h"
#include "pe_bliss2/image/struct_from_va.h"

namespace bv::pe
{

class security_cookie_rule final
	: public core::rule_base<security_cookie_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_security_cookie_rule";
	static constexpr auto reports = output::get_rule_reports<
		security_cookie_rule,
		pe_report::no_security_cookie,
		pe_report::no_security_cookie_for_some_objects,
		pe_report::security_cookie_value_not_default>();

	bool is_applicable(const basic_pe_info& info) const
	{
		return info.has_executable_code && !info.is_boot && !info.is_xbox
			&& !info.is_il_only;
	}

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::image::image& image,
		const pe_bliss::load_config::load_config_directory_details* load_config,
		const pe_bliss::debug::vc_feature_debug_directory_details* vc_feature) const
	{
		if (!load_config)
		{
			reporter.template log<pe_report::no_security_cookie>();
			return;
		}

		bool no_security_cookie = std::visit([](const auto& dir)
		{
			if (!dir.get_descriptor()->base.security_cookie)
				return true;

			return !dir.is_version_at_least(pe_bliss::load_config::version::base);
		}, load_config->get_value());

		if (no_security_cookie)
		{
			reporter.template log<pe_report::no_security_cookie>();
			return;
		}
		else
		{
			std::visit([&reporter, &image](const auto& dir) {
				check_security_cookie_value(reporter, image, dir);
			}, load_config->get_value());
		}

		if (!vc_feature)
			return;

		const auto& descriptor = vc_feature->get_descriptor().get();
		if (!descriptor.gs_count)
		{
			reporter.template log<pe_report::no_security_cookie>();
		}
		else if (descriptor.gs_count != descriptor.c_and_c_plus_plus_count)
		{
			reporter.template log<pe_report::no_security_cookie_for_some_objects>(
				output::named_arg("gs_count", descriptor.gs_count),
				output::named_arg("c_and_c_plus_plus_count",
					descriptor.c_and_c_plus_plus_count));
		}
	}

private:
	template<typename Reporter, typename Dir>
	static void check_security_cookie_value(Reporter& reporter,
		const pe_bliss::image::image& image, const Dir& dir)
	{
		auto security_cookie_va = dir.get_descriptor()->base.security_cookie;
		using cookie_value_type = decltype(security_cookie_va);
		cookie_value_type cookie_value;
		try
		{
			cookie_value = pe_bliss::image::struct_from_va<cookie_value_type>(
				image, security_cookie_va, true).get();
		}
		catch (...)
		{
			reporter.template log<pe_report::security_cookie_value_not_default>(
				output::named_arg("cookie_value", 0u));
			return;
		}

		if constexpr (std::is_same_v<cookie_value_type, std::uint32_t>)
		{
			static constexpr std::uint32_t default_cookie_value_x86_a = 0xbb40e64eu;
			static constexpr std::uint32_t default_cookie_value_x86_b = 0x0000bb40u;
			if (cookie_value != default_cookie_value_x86_a
				&& cookie_value != default_cookie_value_x86_b)
			{
				reporter.template log<pe_report::security_cookie_value_not_default>(
					output::named_arg("cookie_value", cookie_value));
			}
		}
		else
		{
			static constexpr std::uint64_t default_cookie_value_x64 = 0x00002b992ddfa232ull;
			if (cookie_value != default_cookie_value_x64)
				reporter.template log<pe_report::security_cookie_value_not_default>(
					output::named_arg("cookie_value", cookie_value));
		}
	}
};

void security_cookie_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<security_cookie_rule>();
}

} //namespace bv::pe
