#include "binary_valentine/pe/rule/version_info_format_rule.h"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/pe/data/version_info_list.h"
#include "binary_valentine/pe/helpers/version_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/resources/resource_types.h"
#include "pe_bliss2/resources/version_info.h"

namespace bv::pe
{

class version_info_format_rule final
	: public core::rule_base<version_info_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_version_info_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		version_info_format_rule,
		pe_report::version_info_invalid_translation_format,
		pe_report::version_info_invalid_resource_format,
		pe_report::version_info_invalid_block_format,
		pe_report::version_info_duplicate_block,
		pe_report::version_info_duplicate_translations,
		pe_report::version_info_duplicate_strings,
		pe_report::version_info_duplicate_string_languages,
		pe_report::version_info_invalid_string_translations>();

	template<typename Reporter>
	void run(Reporter& reporter, const version_info_list& version_info) const
	{
		for (const auto& info : version_info)
		{
			if (info.block_has_errors)
			{
				log_block_errors(reporter, info.block, info.lang);
				continue;
			}

			if (auto version_info_errors = info.info.get_errors(); version_info_errors)
			{
				for (const auto& err : *version_info_errors)
					log_format_error(reporter, err.first.code, info.lang);
			}
		}
	}

private:
	template<typename Reporter>
	static void log_block_errors(Reporter& reporter,
		const pe_bliss::resources::version_info_block_details& block,
		pe_bliss::resources::resource_id_type lang)
	{
		if (auto block_errors = block.get_errors(); block_errors)
		{
			for (const auto& err : *block_errors)
			{
				version_helpers::log_with_language<
					pe_report::version_info_invalid_block_format>(reporter, lang,
						output::named_arg("message", err.first.code.message()),
						output::named_arg("key", block.get_key()
							? std::u16string_view(block.get_key()->value()) : std::u16string_view{}));
			}
		}

		for (const auto& child : block.get_children())
			log_block_errors(reporter, child, lang);
	}

	template<typename Reporter>
	static void log_format_error(Reporter& reporter,
		const std::error_code& code,
		pe_bliss::resources::resource_id_type lang)
	{
		using enum pe_bliss::resources::version_info_errc;
		using namespace std::literals::string_view_literals;
		if (code == incorrect_root_block_key
			|| code == unknown_version_info_key
			|| code == file_version_info_read_error
			|| code == absent_file_version_info
			|| code == invalid_strings)
		{
			version_helpers::log_with_language<
				pe_report::version_info_invalid_resource_format>(reporter, lang,
					output::named_arg("message", code.message()));
		}
		else if (code == duplicate_string_file_info)
		{
			version_helpers::log_with_language<
				pe_report::version_info_duplicate_block>(reporter, lang,
					output::named_arg("block", "StringFileInfo"sv));
		}
		else if (code == duplicate_var_file_info)
		{
			version_helpers::log_with_language<
				pe_report::version_info_duplicate_block>(reporter, lang,
					output::named_arg("block", "VarFileInfo"sv));
		}
		else if (code == invalid_var_file_info_children
			|| code == invalid_translations
			|| code == absent_translations)
		{
			version_helpers::log_with_language<
				pe_report::version_info_invalid_translation_format>(reporter, lang);
		}
		else if (code == duplicate_translations)
		{
			version_helpers::log_with_language<
				pe_report::version_info_duplicate_translations>(reporter, lang);
		}
		else if (code == duplicate_strings)
		{
			version_helpers::log_with_language<
				pe_report::version_info_duplicate_strings>(reporter, lang);
		}
		else if (code == duplicate_string_translations)
		{
			version_helpers::log_with_language<
				pe_report::version_info_duplicate_string_languages>(reporter, lang);
		}
		else if (code == invalid_string_translations)
		{
			version_helpers::log_with_language<
				pe_report::version_info_invalid_string_translations>(reporter, lang);
		}
	}
};

void version_info_format_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<version_info_format_rule>(shared_values);
}

} //namespace bv::pe
