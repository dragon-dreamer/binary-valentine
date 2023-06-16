#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "binary_valentine/output/result_report_interface.h"

#include "pe_bliss2/resources/lcid.h"
#include "pe_bliss2/resources/resource_types.h"
#include "pe_bliss2/resources/version_info.h"

namespace bv::pe
{

struct persistent_strings
{
	const std::u16string* file_version{};
	const std::u16string* product_version{};
	const std::u16string* original_filename{};
};

class version_helpers final
{
public:
	version_helpers() = delete;

	template<typename Report, typename Reporter, typename... Args>
	static void log_with_language(Reporter& reporter,
		pe_bliss::resources::resource_id_type lang, Args&&... args)
	{
		using namespace std::literals::string_view_literals;
		const auto lcid_info = pe_bliss::resources::get_lcid_info(lang);
		if (lcid_info)
		{
			reporter.template log<Report>(
				output::named_arg("resource_lang", lang),
				output::named_arg("resource_lang_tag", lcid_info->language_tag),
				output::named_arg("resource_lang_location",
					lcid_info->location.empty() ? "-" : lcid_info->location),
				std::forward<Args>(args)...);
		}
		else
		{
			reporter.template log<Report>(
				output::named_arg("resource_lang", lang),
				output::named_arg("resource_lang_tag", "-"sv),
				output::named_arg("resource_lang_location", "-"sv),
				std::forward<Args>(args)...);
		}
	}

	template<typename Report, typename Reporter, typename... Args>
	static void log_with_languages(Reporter& reporter,
		pe_bliss::resources::resource_id_type lang1,
		pe_bliss::resources::resource_id_type lang2,
		Args&&... args)
	{
		const auto lcid_info1 = pe_bliss::resources::get_lcid_info(lang1);
		const auto lcid_info2 = pe_bliss::resources::get_lcid_info(lang2);
		reporter.template log<Report>(
			output::named_arg("resource_lang1", lang1),
			output::named_arg("resource_lang1_tag",
				lcid_info1 ? lcid_info1->language_tag : "-"),
			output::named_arg("resource_lang1_location",
				lcid_info1 && !lcid_info1->location.empty() ? lcid_info1->location : "-"),
			output::named_arg("resource_lang2", lang2),
			output::named_arg("resource_lang2_tag",
				lcid_info2 ? lcid_info2->language_tag : "-"),
			output::named_arg("resource_lang2_location",
				lcid_info2 && !lcid_info2->location.empty() ? lcid_info2->location : "-"),
			std::forward<Args>(args)...);
	}

	static std::vector<std::u16string_view> get_sorted_string_keys(
		const pe_bliss::resources::string_value_map_type& strings)
	{
		std::vector<std::u16string_view> keys;
		keys.reserve(strings.begin()->second.size());
		for (const auto& [key, str] : strings)
			keys.emplace_back(key);

		std::ranges::sort(keys);
		return keys;
	}

	template<typename ContentsMicmatchReport, typename Reporter>
	static void check_strings_mismatch(Reporter& reporter,
		const std::u16string* const string1,
		const std::u16string* const string2,
		pe_bliss::resources::resource_id_type lang,
		std::u16string_view key)
	{
		if (string1 && string2 && *string1 != *string2)
		{
			log_with_language<ContentsMicmatchReport>(reporter, lang,
				output::named_arg("string1", *string1),
				output::named_arg("string2", *string2),
				output::named_arg("key", key));
		}
	}

	template<typename ContentsMicmatchReport, typename Reporter>
	static void check_strings_mismatch(Reporter& reporter,
		const std::u16string* const string1,
		const std::u16string* const string2,
		pe_bliss::resources::resource_id_type lang1,
		pe_bliss::resources::resource_id_type lang2,
		std::u16string_view key)
	{
		if (string1 && string2 && *string1 != *string2)
		{
			log_with_languages<ContentsMicmatchReport>(
				reporter, lang1, lang2,
				output::named_arg("string1", *string1),
				output::named_arg("string2", *string2),
				output::named_arg("key", key));
		}
	}
	
	[[nodiscard]]
	static persistent_strings get_persistent_strings(
		const pe_bliss::resources::string_value_map_type& strings)
	{
		auto end = strings.end();
		auto file_version_it = strings.find(pe_bliss::resources::keys::file_version_key);
		auto product_version_it = strings.find(
			pe_bliss::resources::keys::product_version_key);
		auto original_filename_it = strings.find(
			pe_bliss::resources::keys::original_filename_key);
		return {
			.file_version = file_version_it == end
				? nullptr : &file_version_it->second,
			.product_version = product_version_it == end
				? nullptr : &product_version_it->second,
			.original_filename = original_filename_it == end
				? nullptr : &original_filename_it->second,
		};
	}
};

} //namespace bv::pe
