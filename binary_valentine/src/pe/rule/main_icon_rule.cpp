#include "binary_valentine/pe/rule/main_icon_rule.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <unordered_map>

#include "binary_valentine/core/rule.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/output/rule_report.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/main_pe_icon.h"
#include "binary_valentine/pe/pe_rule_reports.h"
#include "binary_valentine/rule_class.h"
#include "binary_valentine/string/encoding.h"

#include "pe_bliss2/resources/icon_cursor_reader.h"

namespace bv::pe
{

class main_icon_rule final
	: public core::rule_base<main_icon_rule>
{
public:
	static constexpr auto rule_class = rule_class_type::pe;
	static constexpr std::string_view rule_name = "pe_main_icon_rule";
	static constexpr auto reports = output::get_rule_reports<
		main_icon_rule,
		pe_report::no_main_icon,
		pe_report::main_icon_height_width_mismatch,
		pe_report::main_icon_duplicate_size,
		pe_report::main_icon_missing_size,
		pe_report::main_icon_too_low_bit_count,
		pe_report::multilingual_main_icon,
		pe_report::main_icon_format_error>();

	using bit_count_to_icon_count_map_type = std::unordered_map<std::uint16_t, std::size_t>;
	using found_sizes_map_type = std::unordered_map<std::uint16_t, bit_count_to_icon_count_map_type>;

	bool is_applicable(const basic_pe_info& info) const
	{
		return !info.is_xbox && !info.is_resource_only && !info.is_boot
			&& info.is_executable && !info.is_driver
			&& info.file_extension == ".exe";
	}

	template<typename Reporter>
	void run(Reporter& reporter, const main_pe_icon& main_icon) const
	{
		if (!main_icon.icon)
		{
			if (main_icon.error)
			{
				reporter.template log<pe_report::main_icon_format_error>(
					output::named_arg(output::arg::exception, main_icon.error));
			}
			else
			{
				reporter.template log<pe_report::no_main_icon>();
			}
			return;
		}

		if (main_icon.multilingual)
		{
			reporter.template log<pe_report::multilingual_main_icon>(
				output::named_arg("icon", icon_name_id_to_string(main_icon)));
		}

		const auto found_sizes = fetch_icon_sizes(reporter, main_icon);
		check_required_sizes(reporter, found_sizes, main_icon);
		check_required_sizes_bit_count(reporter, found_sizes, main_icon);
	}

private:
	static std::string icon_name_id_to_string(const main_pe_icon& icon)
	{
		try
		{
			const auto* id = std::get_if<std::uint32_t>(&icon.icon_id_name);
			if (id)
				return std::to_string(*id);

			return string::to_utf8(std::get<std::u16string>(icon.icon_id_name));
		}
		catch (const std::exception&)
		{
			return "-";
		}
	}

	template<typename Reporter>
	static void check_required_sizes_bit_count(Reporter& reporter,
		const found_sizes_map_type& sizes,
		const main_pe_icon& icon)
	{
		static constexpr auto high_bit_count_icon_sizes{ std::to_array<std::uint16_t>({
			48u, 256u
		}) };

		for (auto size : high_bit_count_icon_sizes)
		{
			if (auto it = sizes.find(size); it != sizes.cend())
			{
				bool found_suitable_bit_count = false;
				for (const auto& bit_count : it->second)
				{
					if (bit_count.first >= 32u)
					{
						found_suitable_bit_count = true;
						break;
					}
				}

				if (!found_suitable_bit_count)
				{
					reporter.template log<pe_report::main_icon_too_low_bit_count>(
						output::named_arg("icon", icon_name_id_to_string(icon)),
						output::named_arg("height", size),
						output::named_arg("width", size),
						output::named_arg("bit_count", 32u));
				}
			}
		}
	}

	template<typename Reporter>
	static void check_required_sizes(Reporter& reporter,
		const found_sizes_map_type& sizes,
		const main_pe_icon& icon)
	{
		static constexpr auto required_icon_sizes{ std::to_array<std::uint16_t>({
			16u, 32u, 48u, 256u
		}) };

		for (auto size : required_icon_sizes)
		{
			if (!sizes.contains(size))
			{
				reporter.template log<pe_report::main_icon_missing_size>(
					output::named_arg("icon", icon_name_id_to_string(icon)),
					output::named_arg("height", size),
					output::named_arg("width", size));
				break;
			}
		}
	}

	template<typename Reporter>
	static found_sizes_map_type fetch_icon_sizes(Reporter& reporter,
		const main_pe_icon& icon)
	{
		found_sizes_map_type found_sizes;
		for (const auto& header : icon.icon.value().get_resource_group_headers())
		{
			if (header.get_height() != header.get_width())
			{
				reporter.template log<pe_report::main_icon_height_width_mismatch>(
					output::named_arg("icon", icon_name_id_to_string(icon)),
					output::named_arg("height", header.get_height()),
					output::named_arg("width", header.get_width()));
			}
			else
			{
				if (++found_sizes[
					static_cast<std::uint16_t>(header.get_height())][header.get_bit_count()] == 2u)
				{
					reporter.template log<pe_report::main_icon_duplicate_size>(
						output::named_arg("icon", icon_name_id_to_string(icon)),
						output::named_arg("height", header.get_height()),
						output::named_arg("width", header.get_width()),
						output::named_arg("bit_count", header.get_bit_count()));
				}
			}
		}
		return found_sizes;
	}
};

void main_icon_rule_factory::add_rule(core::rule_list& rules,
	core::value_provider_interface& shared_values)
{
	rules.register_rule<main_icon_rule>(shared_values);
}

} //namespace bv::pe
