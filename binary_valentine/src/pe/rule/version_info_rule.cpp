#include "binary_valentine/pe/rule/version_info_rule.h"

#include <cstddef>
#include <iterator>
#include <format>
#include <optional>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <utility>

#include <boost/lexical_cast.hpp>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/version_info_list.h"
#include "binary_valentine/pe/helpers/version_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/resources/cpid.h"
#include "pe_bliss2/resources/lcid.h"
#include "pe_bliss2/resources/resource_directory.h"

namespace bv::pe
{

namespace
{
struct parsed_version
{
	std::uint16_t major{};
	std::uint16_t minor{};
	std::optional<std::uint16_t> build;
	std::optional<std::uint16_t> revision;
	bool has_tail{};
};
} //namespace

class version_info_rule final
	: public core::rule_base<version_info_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_version_info_rule";
	static constexpr auto reports = output::get_rule_reports<
		version_info_rule,
		pe_report::version_info_absent,
		pe_report::version_info_inferred,
		pe_report::version_info_non_release,
		pe_report::version_info_debug,
		pe_report::version_info_private_build,
		pe_report::version_info_string_file_info_absent,
		pe_report::version_info_string_file_info_lcid_resource_lang_mismatch,
		pe_report::version_info_translations_absent,
		pe_report::version_info_translation_lcid_resource_lang_mismatch,
		pe_report::version_info_translations_strings_mismatch,
		pe_report::version_info_private_build_absent,
		pe_report::version_info_special_build_absent,
		pe_report::version_info_private_build_present,
		pe_report::version_info_special_build_present,
		pe_report::version_info_fixed_file_version_string_file_version_mismatch,
		pe_report::version_info_fixed_product_version_string_product_version_mismatch,
		pe_report::version_info_version_string_not_parsable,
		pe_report::version_info_version_string_has_tail,
		pe_report::version_info_cross_string_presence_mismatch,
		pe_report::version_info_cross_string_file_info_mismatch,
		pe_report::version_info_incorrect_file_type,
		pe_report::version_info_default,
		pe_report::version_info_original_filename_mismatch,
		pe_report::version_info_required_string_absent,
		pe_report::version_info_required_string_empty
	>();

	template<typename Reporter>
	void run(Reporter& reporter, const version_info_list* version_info,
		const basic_pe_info& info,
		const std::shared_ptr<const core::subject_entity_interface>& entity) const
	{
		if (!version_info)
		{
			reporter.template log<pe_report::version_info_absent>();
			return;
		}
		
		const auto u16name = string::utf8_to<std::u16string>::convert(info.file_name);
		for (const auto& item : *version_info)
		{
			check_single_version_info_item(reporter, item, info.file_extension,
				u16name);
		}
	}

	template<typename Reporter>
	static void check_basic_file_flags(Reporter& reporter,
		pe_bliss::resources::file_flags::value file_flags,
		pe_bliss::resources::resource_id_type lang)
	{
		using enum pe_bliss::resources::file_flags::value;
		if (file_flags & infoinferred)
		{
			version_helpers::log_with_language<
				pe_report::version_info_inferred>(reporter, lang);
		}

		if (file_flags & prerelease)
		{
			version_helpers::log_with_language<
				pe_report::version_info_non_release>(reporter, lang);
		}

		if (file_flags & debug)
		{
			version_helpers::log_with_language<
				pe_report::version_info_debug>(reporter, lang);
		}

		if (file_flags & privatebuild)
		{
			version_helpers::log_with_language<
				pe_report::version_info_private_build>(reporter, lang);
		}
	}

	template<typename Reporter>
	static void check_strings_lang(Reporter& reporter,
		const pe_bliss::resources::translation_string_map_type& strings,
		pe_bliss::resources::resource_id_type lang)
	{
		if (strings.empty())
		{
			version_helpers::log_with_language<
				pe_report::version_info_string_file_info_absent>(reporter, lang);
		}
		else if (strings.size() == 1u)
		{
			auto string_file_info_lcid = strings.begin()->first.lcid;
			if (string_file_info_lcid && string_file_info_lcid != lang)
			{
				const auto string_lcid_info = pe_bliss::resources::get_lcid_info(string_file_info_lcid);

				using namespace std::literals::string_view_literals;
				if (string_lcid_info)
				{
					version_helpers::log_with_language<
						pe_report::version_info_string_file_info_lcid_resource_lang_mismatch>(
							reporter, lang,
							output::named_arg("string_file_info_lang", string_file_info_lcid),
							output::named_arg("string_file_info_lang_tag", string_lcid_info->language_tag),
							output::named_arg("string_file_info_lang_location",
								string_lcid_info->location.empty() ? empty_ : string_lcid_info->location));
				}
				else
				{
					version_helpers::log_with_language<
						pe_report::version_info_string_file_info_lcid_resource_lang_mismatch>(
							reporter, lang,
							output::named_arg("string_file_info_lang", string_file_info_lcid),
							output::named_arg("string_file_info_lang_tag", empty_),
							output::named_arg("string_file_info_lang_location", empty_));
				}
			}
		}
	}

	template<typename Reporter>
	static void check_translations_lang(Reporter& reporter,
		const pe_bliss::resources::translation_set& translations,
		pe_bliss::resources::resource_id_type lang)
	{
		if (translations.empty())
		{
			version_helpers::log_with_language<
				pe_report::version_info_translations_absent>(reporter, lang);
		}
		else if (translations.size() == 1u)
		{
			auto translation_lcid = translations.begin()->lcid;
			if (translation_lcid && translation_lcid != lang)
			{
				using namespace std::literals::string_view_literals;
				const auto translation_lcid_info = pe_bliss::resources::get_lcid_info(translation_lcid);
				if (translation_lcid_info)
				{
					version_helpers::log_with_language<
						pe_report::version_info_translation_lcid_resource_lang_mismatch>(
							reporter, lang,
							output::named_arg("translations_lang", translation_lcid),
							output::named_arg("translations_lang_tag", translation_lcid_info->language_tag),
							output::named_arg("translations_lang_location",
								translation_lcid_info->location.empty()
								? empty_ : translation_lcid_info->location));
				}
				else
				{
					version_helpers::log_with_language<
						pe_report::version_info_translation_lcid_resource_lang_mismatch>(
							reporter, lang,
							output::named_arg("translations_lang", translation_lcid),
							output::named_arg("translations_lang_tag", empty_),
							output::named_arg("translations_lang_location", empty_));
				}
			}
		}
	}

	template<typename Reporter>
	static void check_strings_translations_match(Reporter& reporter,
		const pe_bliss::resources::translation_string_map_type& strings,
		const pe_bliss::resources::translation_set& translations,
		pe_bliss::resources::resource_id_type lang)
	{
		if (strings.size() != translations.size())
		{
			version_helpers::log_with_language<
				pe_report::version_info_translations_strings_mismatch>(reporter, lang);
		}
		else
		{
			for (const auto& translation : translations)
			{
				if (!strings.contains(translation))
				{
					version_helpers::log_with_language<
						pe_report::version_info_translations_strings_mismatch>(reporter, lang);
					break;
				}
			}
		}
	}

	template<typename Report, typename Reporter, typename... Args>
	static void log_with_translation(Reporter& reporter,
		const pe_bliss::resources::translation& translation,
		Args&&... args)
	{
		const auto lcid_info = pe_bliss::resources::get_lcid_info(translation.lcid);
		const auto cpid_name = pe_bliss::resources::get_cpid_name(
			static_cast<pe_bliss::resources::code_page>(translation.cpid));

		reporter.template log<Report>(
			output::named_arg("string_lcid", translation.lcid),
			output::named_arg("string_lcid_tag", lcid_info ? lcid_info->language_tag : empty_),
			output::named_arg("string_lcid_location",
				lcid_info && !lcid_info->location.empty() ? lcid_info->location : empty_),
			output::named_arg("string_cpid", translation.cpid),
			output::named_arg("string_cpid_name", cpid_name ? *cpid_name : empty_),
			std::forward<Args>(args)...);
	}

	template<typename Report, typename Reporter>
	static auto check_string_present(Reporter& reporter,
		std::u16string_view key,
		const pe_bliss::resources::string_value_map_type& strings,
		const pe_bliss::resources::translation& translation,
		bool is_expected = true)
	{
		auto it = strings.find(key);
		if ((it != strings.end()) != is_expected)
			log_with_translation<Report>(reporter, translation);

		return it;
	}

	static std::optional<parsed_version> parse_version(const std::u16string& str)
	{
		std::optional<parsed_version> result;
		auto utf8_version_string = string::to_utf8(str);
		std::smatch match;
		if (std::regex_match(utf8_version_string, match, version_regex_))
		{
			auto& ver = result.emplace();
			ver.major = boost::lexical_cast<std::uint16_t>(match[1].str());
			ver.minor = boost::lexical_cast<std::uint16_t>(match[2].str());
			if (match[3].matched)
				ver.build = boost::lexical_cast<std::uint16_t>(match[3].str());
			if (match[4].matched)
				ver.revision = boost::lexical_cast<std::uint16_t>(match[4].str());
			ver.has_tail = match[5].matched && match[5].first != match[5].second;
		}

		return result;
	}

	template<typename Reporter>
	static bool version_matches(
		Reporter& reporter,
		pe_bliss::resources::string_value_map_type::const_iterator version_string,
		const pe_bliss::resources::full_version& fixed_version,
		const pe_bliss::resources::string_value_map_type& strings,
		const pe_bliss::resources::translation& translation)
	{
		if (version_string == strings.end())
			return true;

		std::optional<parsed_version> parsed_ver;
		try
		{
			parsed_ver = parse_version(version_string->second);
		}
		catch (const std::exception&)
		{
			parsed_ver.reset();
		}

		if (!parsed_ver)
		{
			log_with_translation<pe_report::version_info_version_string_not_parsable>(
				reporter, translation,
				output::named_arg("version_string", version_string->second));
			return true;
		}

		if (parsed_ver->has_tail)
		{
			log_with_translation<pe_report::version_info_version_string_has_tail>(
				reporter, translation,
				output::named_arg("version_string", version_string->second));
		}

		if (parsed_ver->major != fixed_version.major
			|| parsed_ver->minor != fixed_version.minor)
		{
			return false;
		}

		if (parsed_ver->build.has_value()
			&& parsed_ver->build.value() != fixed_version.build)
		{
			return false;
		}

		if (parsed_ver->revision.has_value()
			&& parsed_ver->revision.value() != fixed_version.revision)
		{
			return false;
		}

		return true;
	}

	template<typename Report, typename Reporter>
	static void check_version_match(Reporter& reporter,
		pe_bliss::resources::string_value_map_type::const_iterator version_string,
		const pe_bliss::resources::full_version& fixed_version,
		const pe_bliss::resources::string_value_map_type& strings,
		const pe_bliss::resources::translation& translation)
	{
		if (version_matches(reporter, version_string,
			fixed_version, strings, translation))
		{
			return;
		}

		auto fixed_version_string = std::format("{}.{}.{}.{}",
			fixed_version.major, fixed_version.minor,
			fixed_version.build, fixed_version.revision);
		log_with_translation<Report>(reporter, translation,
			output::named_arg("version_string", version_string->second),
			output::named_arg("fixed_version", std::move(fixed_version_string)));
	}
	
	template<typename Reporter>
	static void check_cross_string_match(Reporter& reporter,
		const pe_bliss::resources::translation_string_map_type& strings,
		pe_bliss::resources::resource_id_type res_lang)
	{
		if (strings.size() < 2u)
			return;

		auto first_strings = version_helpers::get_persistent_strings(strings.begin()->second);
		for (const auto& [lang, lang_strings] : strings | std::views::drop(1))
		{
			auto current_strings = version_helpers::get_persistent_strings(lang_strings);
			version_helpers::check_strings_mismatch<
				pe_report::version_info_cross_string_file_info_mismatch>(reporter,
					first_strings.file_version, current_strings.file_version, res_lang,
					pe_bliss::resources::keys::file_version_key);
			version_helpers::check_strings_mismatch<
				pe_report::version_info_cross_string_file_info_mismatch>(reporter,
					first_strings.product_version, current_strings.product_version, res_lang,
					pe_bliss::resources::keys::product_version_key);
			version_helpers::check_strings_mismatch<
				pe_report::version_info_cross_string_file_info_mismatch>(reporter,
					first_strings.original_filename, current_strings.original_filename, res_lang,
					pe_bliss::resources::keys::original_filename_key);
		}

		const auto first_keys = version_helpers::get_sorted_string_keys(strings.begin()->second);
		std::vector<std::u16string_view> mismatch;
		for (const auto& [lang, lang_strings] : strings | std::views::drop(1))
		{
			const auto current_keys = version_helpers::get_sorted_string_keys(lang_strings);
			std::ranges::set_symmetric_difference(first_keys, current_keys,
				std::back_inserter(mismatch));
			for (const auto& key : mismatch)
			{
				version_helpers::log_with_language<
					pe_report::version_info_cross_string_presence_mismatch>(
						reporter, res_lang,
						output::named_arg("key", key));
			}
			mismatch.clear();
		}
	}

	template<typename Reporter>
	static void check_single_version_info_item(Reporter& reporter,
		const version_info_item& item, const std::string& file_extension,
		std::u16string_view file_name)
	{
		if (item.block_has_errors || item.info.has_errors())
			return;

		const auto& file_version = item.info.get_file_version_info();
		check_file_type(reporter, file_version.get_file_type(), file_extension, item.lang);

		const auto file_flags = file_version.get_file_flags();
		check_basic_file_flags(reporter, file_flags, item.lang);
		check_strings_lang(reporter, item.info.get_strings(), item.lang);
		check_translations_lang(reporter, item.info.get_translations(), item.lang);
		check_strings_translations_match(reporter, item.info.get_strings(),
			item.info.get_translations(), item.lang);
		check_cross_string_match(reporter, item.info.get_strings(), item.lang);

		//Check if required strings are present
		//https://learn.microsoft.com/en-us/windows/win32/menurc/versioninfo-resource
		for (const auto& [lang, lang_strings] : item.info.get_strings())
		{
			if (auto it = lang_strings.find(pe_bliss::resources::keys::original_filename_key);
				it != lang_strings.end())
			{
				if (!it->second.empty() && it->second != file_name)
				{
					log_with_translation<pe_report::version_info_original_filename_mismatch>(
						reporter, lang,
						output::named_arg(output::arg::entity_name, file_name),
						output::named_arg("original_file_name", it->second));
				}
			}

			if (file_flags & pe_bliss::resources::file_flags::privatebuild)
			{
				check_string_present<pe_report::version_info_private_build_absent>(
					reporter, pe_bliss::resources::keys::private_build_key, lang_strings, lang);
			}
			else
			{
				check_string_present<pe_report::version_info_private_build_present>(
					reporter, pe_bliss::resources::keys::private_build_key, lang_strings, lang,
					false);
			}

			if (file_flags & pe_bliss::resources::file_flags::specialbuild)
			{
				check_string_present<pe_report::version_info_special_build_absent>(
					reporter, pe_bliss::resources::keys::special_build_key, lang_strings, lang);
			}
			else
			{
				check_string_present<pe_report::version_info_special_build_present>(
					reporter, pe_bliss::resources::keys::special_build_key, lang_strings, lang,
					false);
			}

			check_version_match<pe_report::version_info_fixed_file_version_string_file_version_mismatch>(
				reporter,
				lang_strings.find(pe_bliss::resources::keys::file_version_key),
				file_version.get_file_version(), lang_strings, lang);
			check_version_match<pe_report::version_info_fixed_product_version_string_product_version_mismatch>(
				reporter,
				lang_strings.find(pe_bliss::resources::keys::product_version_key),
				file_version.get_product_version(), lang_strings, lang);

			check_default_version_info(reporter, lang_strings, lang);
			check_empty_version_info(reporter, lang_strings, lang);
		}
	}

	template<typename Reporter>
	static void check_default_version_info(Reporter& reporter,
		const pe_bliss::resources::string_value_map_type& strings,
		const pe_bliss::resources::translation& translation)
	{
		using namespace std::string_view_literals;
		static constexpr std::array keys{
			pe_bliss::resources::keys::company_name_key,
			pe_bliss::resources::keys::file_description_key,
			pe_bliss::resources::keys::product_name_key,
			pe_bliss::resources::keys::program_id
		};

		for (const auto& key : keys)
		{
			auto it = strings.find(key);
			if (it == strings.end())
				continue;

			if (it->second.find(u"TODO:") != std::u16string::npos
				|| it->second.find(u"com.embarcadero.") != std::u16string::npos)
			{
				log_with_translation<pe_report::version_info_default>(
					reporter, translation,
					output::named_arg("key", key),
					output::named_arg("value", it->second));
			}
		}
	}

	template<typename Reporter>
	static void check_empty_version_info(Reporter& reporter,
		const pe_bliss::resources::string_value_map_type& strings,
		const pe_bliss::resources::translation& translation)
	{
		using namespace std::string_view_literals;
		static constexpr std::array keys{
			pe_bliss::resources::keys::company_name_key,
			pe_bliss::resources::keys::file_description_key,
			pe_bliss::resources::keys::file_version_key,
			pe_bliss::resources::keys::internal_name_key,
			pe_bliss::resources::keys::original_filename_key,
			pe_bliss::resources::keys::product_name_key,
			pe_bliss::resources::keys::product_version_key
		};

		for (const auto& key : keys)
		{
			auto it = strings.find(key);
			if (it == strings.end())
			{
				log_with_translation<pe_report::version_info_required_string_absent>(
					reporter, translation,
					output::named_arg("key", key));
				continue;
			}

			if (it->second.empty())
			{
				log_with_translation<pe_report::version_info_required_string_empty>(
					reporter, translation,
					output::named_arg("key", key));
			}
		}
	}

	static std::string get_file_type_name(pe_bliss::resources::file_type type)
	{
		using enum pe_bliss::resources::file_type;
		switch (type)
		{
		case unknown: return "VFT_UNKNOWN";
		case app: return "VFT_APP";
		case dll: return "VFT_DLL";
		case drv: return "VFT_DRV";
		case font: return "VFT_FONT";
		case vxd: return "VFT_VXD";
		case static_lib: return "VFT_STATIC_LIB";
		}
		return std::to_string(static_cast<std::uint32_t>(type));
	}

	template<typename Reporter>
	static void check_file_type(Reporter& reporter,
		pe_bliss::resources::file_type type,
		const std::string& file_extension,
		pe_bliss::resources::resource_id_type lang)
	{
		using namespace std::string_view_literals;
		if (file_extension == ".exe")
		{
			if (type != pe_bliss::resources::file_type::app)
			{
				version_helpers::log_with_language<
					pe_report::version_info_incorrect_file_type>(
						reporter, lang,
						output::named_arg("extension", file_extension),
						output::named_arg("current_type", get_file_type_name(type)),
						output::named_arg("expected_type", "VFT_APP"sv));
			}
		}
		else if (file_extension == ".dll" || file_extension == ".ocx")
		{
			if (type != pe_bliss::resources::file_type::dll)
			{
				version_helpers::log_with_language<
					pe_report::version_info_incorrect_file_type>(
						reporter, lang,
						output::named_arg("extension", file_extension),
						output::named_arg("current_type", get_file_type_name(type)),
						output::named_arg("expected_type", "VFT_DLL"sv));
			}
		}
		else if (file_extension == ".sys")
		{
			if (type != pe_bliss::resources::file_type::dll
				&& type != pe_bliss::resources::file_type::drv)
			{
				version_helpers::log_with_language<
					pe_report::version_info_incorrect_file_type>(
						reporter, lang,
						output::named_arg("extension", file_extension),
						output::named_arg("current_type", get_file_type_name(type)),
						output::named_arg("expected_type", "VFT_DLL/VFT_DRV"sv));
			}
		}
	}

private:
	static const inline std::regex version_regex_{ "(\\d+).(\\d+)(?:.(\\d+)(?:.(\\d+))?)?(.*?)",
		std::regex_constants::ECMAScript | std::regex_constants::optimize };
	static constexpr std::string_view empty_{ "-" };
};

void version_info_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<version_info_rule>(shared_values);
}

} //namespace bv::pe
