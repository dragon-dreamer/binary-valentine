#include "binary_valentine/pe/combined_rule/combined_version_info_rule.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <optional>
#include <ranges>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/core/combined_rule.h"
#include "binary_valentine/core/hash_combine.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/version_info_list.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/resources/version_info.h"

namespace bv::pe
{

class combined_version_info_rule final
	: public core::combined_rule_base<combined_version_info_rule>
{
public:
	using dependencies_type = core::combined_dependencies<
		core::filtered_value<const version_info_list*>>;
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_combined_version_info_rule";
	static constexpr auto reports = output::get_rule_reports<
		combined_version_info_rule,
		pe_report::version_info_fixed_product_version_cross_image_mismatch,
		pe_report::version_info_translations_cross_image_mismatch,
		pe_report::version_info_strings_cross_image_mismatch>();

	void run(auto&& version_infos) const
	{
		check_fixed_product_versions(version_infos);
		check_translations(version_infos);
		check_strings(version_infos);
	}

private:
	using translation_vector = std::vector<pe_bliss::resources::translation>;

	struct full_version_hash
	{
		std::size_t operator()(const pe_bliss::resources::full_version& ver) const
		{
			std::size_t seed{};
			core::hash_combine(seed, ver.build, ver.major, ver.minor, ver.revision);
			return seed;
		}
	};

	template<typename Map>
	static const typename Map::key_type* get_most_popular_item(const Map& map)
	{
		auto max = std::max_element(map.begin(),
			map.end(), [](const auto& l, const auto& r) {
			return l.second < r.second;
		});

		if (max == map.end())
			return nullptr;

		return &max->first;
	}

	struct translation_vector_hash
	{
		template<typename Vector>
		std::size_t operator()(const Vector& vector) const
		{
			std::size_t hash = 0;
			for (const auto& elem : vector)
				core::hash_combine(hash, elem);
			return hash;
		}
	};
	
	struct string_info
	{
		std::u16string_view string_id;
		std::unordered_map<std::optional<std::u16string_view>,
			std::size_t> string_to_count;
	};

	static const pe_bliss::resources::string_value_map_type* find_strings_by_translation(
		const version_info_list* infos, const pe_bliss::resources::translation& translation)
	{
		if (!infos)
			return nullptr;

		for (const auto& info : *infos)
		{
			if (info.block_has_errors || info.info.has_errors())
				continue;

			auto it = info.info.get_strings().find(translation);
			if (it != info.info.get_strings().end())
				return &it->second;
		}

		return nullptr;
	}

	static void check_strings(auto&& version_infos)
	{
		const auto most_frequent_translation = get_most_popular_translation(version_infos);
		if (!most_frequent_translation)
			return;

		auto similar_strings = std::to_array<string_info>({
			{ pe_bliss::resources::keys::company_name_key },
			{ pe_bliss::resources::keys::product_name_key },
			{ pe_bliss::resources::keys::product_version_key },
			{ pe_bliss::resources::keys::legal_copyright_key },
			{ pe_bliss::resources::keys::legal_trademarks_key }
		});

		std::vector<std::pair<const pe_bliss::resources::string_value_map_type*,
			output::rule_report_helper<combined_version_info_rule>>> strings_to_reporter;
		for (const auto& [infos, reporter] : version_infos)
		{
			const auto* strings = find_strings_by_translation(
				infos, *most_frequent_translation);
			if (!strings)
				continue;
			
			strings_to_reporter.emplace_back(strings, reporter);
			for (auto& [string_id, string_to_count] : similar_strings)
			{
				auto it = strings->find(string_id);
				if (it == strings->end())
					++string_to_count[std::nullopt];
				else
					++string_to_count[it->second];
			}
		}

		static constexpr std::u16string_view empty{ u"-" };
		for (const auto& [string_id, string_to_count] : similar_strings)
		{
			const auto* most_popular_string = get_most_popular_item(string_to_count);
			if (!most_popular_string)
				continue;

			for (auto& [strings, reporter] : strings_to_reporter)
			{
				std::optional<std::u16string_view> string;
				auto it = strings->find(string_id);
				if (it != strings->end())
					string = it->second;

				if (*most_popular_string != string)
				{
					reporter.template log<
						pe_report::version_info_strings_cross_image_mismatch>(
							output::named_arg("string_id", string_id),
							output::named_arg("most_popular_string",
								*most_popular_string ? **most_popular_string : empty),
							output::named_arg("string", string ? *string : empty));
				}
			}
		}
	}

	static std::optional<pe_bliss::resources::translation>
		get_most_popular_translation(auto&& version_infos)
	{
		std::unordered_map<pe_bliss::resources::translation, std::size_t> translation_to_count;
		for (const auto& [infos, reporter] : version_infos)
		{
			if (!infos)
				continue;

			for (const auto& info : *infos)
			{
				if (info.block_has_errors || info.info.has_errors())
					continue;

				for (const auto& [translation, strings] : info.info.get_strings())
					++translation_to_count[translation];
			}
		}

		std::optional<pe_bliss::resources::translation> result;
		const auto* most_popular_translation = get_most_popular_item(translation_to_count);
		if (most_popular_translation)
			result = *most_popular_translation;
		return result;
	}

	static std::string translation_vector_to_string(const translation_vector& translations)
	{
		std::string result;
		for (const auto& translation : translations)
		{
			result.append(std::format("{:04x}{:04x}, ",
				translation.lcid, translation.cpid));
		}
		if (!result.empty())
			result.resize(result.size() - 2u); //Cut last ", "
		return result;
	}

	static void check_translations(auto&& version_infos)
	{
		std::unordered_map<translation_vector, std::size_t, translation_vector_hash>
			translation_vector_to_count;
		std::vector<std::pair<translation_vector,
			output::rule_report_helper<combined_version_info_rule>>> translation_vector_to_reporter;
		for (const auto& [infos, reporter] : version_infos)
		{
			if (!infos)
				continue;

			translation_vector translations;
			translations.reserve(infos->size());
			for (const auto& info : *infos)
			{
				if (info.block_has_errors || info.info.has_errors())
					continue;

				for (const auto& [translation, strings] : info.info.get_strings())
					translations.emplace_back(translation);
			}

			std::ranges::sort(translations);
			++translation_vector_to_count[translations];
			translation_vector_to_reporter.emplace_back(std::move(translations), reporter);
		}

		const auto* most_popular_translations = get_most_popular_item(
			translation_vector_to_count);
		if (!most_popular_translations)
			return;

		std::optional<std::string> most_popular_translations_string;
		for (auto& [translations, reporter] : translation_vector_to_reporter)
		{
			if (translations != *most_popular_translations)
			{
				if (!most_popular_translations_string)
				{
					most_popular_translations_string = translation_vector_to_string(
						*most_popular_translations);
				}

				reporter.template log<
					pe_report::version_info_translations_cross_image_mismatch>(
						output::named_arg("most_popular_translations",
							most_popular_translations_string.value()),
						output::named_arg("translations",
							translation_vector_to_string(translations)));
			}
		}
	}

	static void check_fixed_product_versions(auto&& version_infos)
	{
		std::unordered_map<pe_bliss::resources::full_version, std::size_t,
			full_version_hash> fixed_product_version_to_count;
		std::vector<std::pair<
			const pe_bliss::resources::version_info_details*,
			output::rule_report_helper<combined_version_info_rule>>> blocks;
		for (const auto& [infos, reporter] : version_infos)
		{
			if (!infos)
				continue;

			auto first_block = find_first_valid_block(*infos);
			if (!first_block)
				continue;

			++fixed_product_version_to_count[
				first_block->get_file_version_info().get_product_version()];
			blocks.emplace_back(first_block, reporter);
		}

		const auto* most_popular_version = get_most_popular_item(
			fixed_product_version_to_count);
		if (!most_popular_version)
			return;

		auto most_popular_version_string = std::format("{}.{}.{}.{}",
			most_popular_version->major,
			most_popular_version->minor,
			most_popular_version->build,
			most_popular_version->revision);

		for (auto& [block, reporter] : blocks)
		{
			if (block->get_file_version_info().get_product_version() != *most_popular_version)
			{
				reporter.template log<
					pe_report::version_info_fixed_product_version_cross_image_mismatch>(
						output::named_arg("most_popular_version",
							most_popular_version_string),
						output::named_arg("version",
							block->get_file_version_info().get_product_version_string())
					);
			}
		}
	}
};

void combined_version_info_rule_factory::add_rule(core::combined_rule_list& rules)
{
	rules.register_rule<combined_version_info_rule>();
}

} //namespace bv::pe
