#include "binary_valentine/pe/rule/cross_resource_version_info_rule.h"

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

namespace bv::pe
{

class cross_resource_version_info_rule final
	: public core::rule_base<cross_resource_version_info_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_cross_resource_version_info_rule";
	static constexpr auto reports = output::get_rule_reports<
		cross_resource_version_info_rule,
		pe_report::version_info_multiple_resources_multiple_translations,
		pe_report::version_info_fixed_file_info_mismatch,
		pe_report::version_info_cross_resource_string_presence_mismatch,
		pe_report::version_info_cross_resource_string_file_info_mismatch>();

	static version_info_list::const_iterator
		get_first_valid_info(const version_info_list& version_info) noexcept
	{
		return std::ranges::find_if(version_info, [](const auto& item) {
			return !item.block_has_errors && !item.info.has_errors();
		});
	}

	template<typename Reporter>
	void run(Reporter& reporter, const version_info_list& version_info) const
	{
		if (version_info.size() < 2u)
			return;

		check_file_version_info_difference(reporter,
			get_first_valid_info(version_info),
			version_info.cend());
	}

	static auto get_file_and_product_version(
		const pe_bliss::resources::translation_string_map_type& strings)
	{
		std::pair<const std::u16string*,
			const std::u16string*> file_product_version_strings;

		if (strings.empty())
			return file_product_version_strings;

		const auto& first_string_map = strings.begin()->second;
		auto it = first_string_map.find(pe_bliss::resources::keys::file_version_key);
		if (it != first_string_map.end())
			file_product_version_strings.first = &it->second;
		it = first_string_map.find(pe_bliss::resources::keys::product_version_key);
		if (it != first_string_map.end())
			file_product_version_strings.second = &it->second;

		return file_product_version_strings;
	}

	template<typename Reporter, typename Value>
	static void check_values_mismatch(Reporter& reporter,
		std::string_view field_name,
		const Value& value1, const Value& value2,
		pe_bliss::resources::resource_id_type lang1,
		pe_bliss::resources::resource_id_type lang2)
	{
		if (value1 != value2)
		{
			version_helpers::log_with_languages<pe_report::version_info_fixed_file_info_mismatch>(
				reporter, lang1, lang2,
				output::named_arg("field_name", field_name),
				output::named_arg("value1", value1),
				output::named_arg("value2", value2));
		}
	}

	template<typename Reporter>
	static void check_string_presence_mismatch(Reporter& reporter,
		const pe_bliss::resources::string_value_map_type& current_string_info,
		pe_bliss::resources::resource_id_type lang1,
		pe_bliss::resources::resource_id_type lang2,
		const std::vector<std::u16string_view>& first_keys,
		std::vector<std::u16string_view>& mismatch)
	{
		const auto current_keys = version_helpers::get_sorted_string_keys(current_string_info);
		std::ranges::set_symmetric_difference(first_keys, current_keys,
			std::back_inserter(mismatch));
		for (const auto& key : mismatch)
		{
			version_helpers::log_with_languages<
				pe_report::version_info_cross_resource_string_presence_mismatch>(
					reporter, lang1, lang2,
					output::named_arg("key", key));
		}
		mismatch.clear();
	}

	template<typename Reporter>
	static void check_persistent_strings_mismatch(Reporter& reporter,
		const pe_bliss::resources::string_value_map_type& current_string_info,
		const persistent_strings& first_persistent_strings,
		pe_bliss::resources::resource_id_type lang1,
		pe_bliss::resources::resource_id_type lang2)
	{
		auto current_strings = version_helpers::get_persistent_strings(current_string_info);
		version_helpers::check_strings_mismatch<
			pe_report::version_info_cross_resource_string_file_info_mismatch>(reporter,
				first_persistent_strings.file_version,
				current_strings.file_version, lang1, lang2,
				pe_bliss::resources::keys::file_version_key);
		version_helpers::check_strings_mismatch<
			pe_report::version_info_cross_resource_string_file_info_mismatch>(reporter,
				first_persistent_strings.product_version,
				current_strings.product_version, lang1, lang2,
				pe_bliss::resources::keys::product_version_key);
		version_helpers::check_strings_mismatch<
			pe_report::version_info_cross_resource_string_file_info_mismatch>(reporter,
				first_persistent_strings.original_filename,
				current_strings.original_filename, lang1, lang2,
				pe_bliss::resources::keys::original_filename_key);
	}

	template<typename Reporter>
	static void check_file_version_info_difference(Reporter& reporter,
		version_info_list::const_iterator first,
		version_info_list::const_iterator last)
	{
		if (first == last)
			return;

		const auto& first_info = first->info.get_file_version_info().get_descriptor();
		const auto& first_strings = first->info.get_strings();
		persistent_strings first_persistent_strings;

		if (first_strings.size() > 1u)
		{
			version_helpers::log_with_language<
				pe_report::version_info_multiple_resources_multiple_translations>(
					reporter, first->lang);
		}

		std::vector<std::u16string_view> first_keys, mismatch;
		if (!first_strings.empty())
		{
			const auto& first_strings_item = first_strings.begin()->second;
			first_keys = version_helpers::get_sorted_string_keys(first_strings_item);
			first_persistent_strings = version_helpers::get_persistent_strings(first_strings_item);
		}

		auto [first_file_version, first_product_version]
			= get_file_and_product_version(first_strings);

		for (auto it = std::next(first); it != last; ++it)
		{
			if (it->block_has_errors || it->info.has_errors())
				continue;

			const auto& current_info = it->info.get_file_version_info().get_descriptor();
			check_values_mismatch(reporter, "FILEFLAGS",
				current_info->file_flags, first_info->file_flags,
				first->lang, it->lang);
			check_values_mismatch(reporter, "FILEOS",
				current_info->file_os, first_info->file_os,
				first->lang, it->lang);
			check_values_mismatch(reporter, "FILESUBTYPE",
				current_info->file_subtype, first_info->file_subtype,
				first->lang, it->lang);
			check_values_mismatch(reporter, "FILETYPE",
				current_info->file_type, first_info->file_type,
				first->lang, it->lang);
			check_values_mismatch(reporter, "FILEDATE",
				(static_cast<std::uint64_t>(current_info->file_date_ms) << 32ull)
					| current_info->file_date_ls,
				(static_cast<std::uint64_t>(first_info->file_date_ms) << 32ull)
					| first_info->file_date_ls,
				first->lang, it->lang);
			check_values_mismatch(reporter, "FILEVERSION",
				(static_cast<std::uint64_t>(current_info->file_version_ms) << 32ull)
					| current_info->file_version_ls,
				(static_cast<std::uint64_t>(first_info->file_version_ms) << 32ull)
					| first_info->file_version_ls,
				first->lang, it->lang);
			check_values_mismatch(reporter, "PRODUCTVERSION",
				(static_cast<std::uint64_t>(current_info->product_version_ms) << 32ull)
					| current_info->product_version_ls,
				(static_cast<std::uint64_t>(first_info->product_version_ms) << 32ull)
					| first_info->product_version_ls,
				first->lang, it->lang);

			const auto& string_info = it->info.get_strings();
			if (string_info.size() > 1u)
			{
				version_helpers::log_with_language<
					pe_report::version_info_multiple_resources_multiple_translations>(
						reporter, it->lang);
			}

			if (string_info.empty())
				continue;

			const auto& current_string_info = string_info.begin()->second;
			check_string_presence_mismatch(reporter,
				current_string_info, first->lang, it->lang, first_keys, mismatch);
			check_persistent_strings_mismatch(reporter,
				current_string_info, first_persistent_strings,
				first->lang, it->lang);
		}
	}
};

void cross_resource_version_info_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<cross_resource_version_info_rule>(shared_values);
}

} //namespace bv::pe
