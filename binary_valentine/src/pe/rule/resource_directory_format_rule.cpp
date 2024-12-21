#include "binary_valentine/pe/rule/resource_directory_format_rule.h"

#include <string_view>
#include <system_error>
#include <unordered_map>
#include <variant>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/helpers/error_helpers.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/error_list.h"
#include "pe_bliss2/resources/resource_directory.h"
#include "pe_bliss2/resources/resource_directory_loader.h"

namespace bv::pe
{

class resource_directory_format_rule final
	: public core::rule_base<resource_directory_format_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_resource_directory_format_rule";
	static constexpr auto reports = output::get_rule_reports<
		resource_directory_format_rule,
		pe_report::resource_directory_format_error,
		pe_report::resource_directory_loop>();

	template<typename Reporter>
	void run(Reporter& reporter,
		const pe_bliss::resources::resource_directory_details& header) const
	{
		bool has_loops = false;
		pe_bliss::error_list merged;
		merge_format_errors(header, merged, has_loops);
		error_helpers::report_errors<pe_report::resource_directory_format_error>(
			reporter, merged, error_code_to_message_id_);

		if (has_loops)
			reporter.template log<pe_report::resource_directory_loop>();
	}

private:
	static constexpr void merge_format_errors(
		std::monostate, const pe_bliss::error_list&, bool) {}

	static void merge_format_errors(
		const pe_bliss::resources::resource_directory_details& dir,
		pe_bliss::error_list& merged,
		bool& has_loops)
	{
		error_helpers::merge_errors(dir, merged);
		for (const auto& entry : dir.get_entries())
			merge_format_errors(entry, merged, has_loops);
	}

	static void merge_format_errors(
		const pe_bliss::resources::resource_directory_entry_details& entry,
		pe_bliss::error_list& merged,
		bool& has_loops)
	{
		error_helpers::merge_errors(entry, merged);

		std::visit([&merged, &has_loops](const auto& value) {
			merge_format_errors(value, merged, has_loops);
		}, entry.get_data_or_directory());
	}

	static void merge_format_errors(
		const pe_bliss::resources::resource_data_entry_details& data,
		pe_bliss::error_list& merged,
		bool)
	{
		error_helpers::merge_errors(data, merged);
	}

	static void merge_format_errors(
		pe_bliss::rva_type,
		const pe_bliss::error_list&,
		bool& has_loops)
	{
		has_loops = true;
	}

	static std::unordered_map<std::error_code, std::string_view> get_ec_to_id_map()
	{
		using enum pe_bliss::resources::resource_directory_loader_errc;

		return {
			{ invalid_directory_size,
				"pe_resources_invalid_directory_size" },
			{ invalid_resource_directory,
				"pe_resources_invalid_resource_directory" },
			{ invalid_resource_directory_number_of_entries,
				"pe_resources_invalid_resource_directory_number_of_entries" },
			{ invalid_resource_directory_entry,
				"pe_resources_invalid_resource_directory_entry" },
			{ invalid_resource_directory_entry_name,
				"pe_resources_invalid_resource_directory_entry_name" },
			{ invalid_number_of_named_and_id_entries,
				"pe_resources_invalid_number_of_named_and_id_entries" },
			{ invalid_resource_data_entry,
				"pe_resources_invalid_resource_data_entry" },
			{ invalid_resource_data_entry_raw_data,
				"pe_resources_invalid_resource_data_entry_raw_data" },
			{ unsorted_entries, "pe_resources_unsorted_entries" },
			{ duplicate_entries, "pe_resources_duplicate_entries" }
		};
	}

private:
	static const inline std::unordered_map<
		std::error_code, std::string_view> error_code_to_message_id_ = get_ec_to_id_map();
};

void resource_directory_format_rule_factory::add_rule(core::rule_list& rules)
{
	rules.register_rule<resource_directory_format_rule>();
}

} //namespace bv::pe
