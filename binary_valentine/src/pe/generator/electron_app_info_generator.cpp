#include "binary_valentine/pe/generator/electron_app_info_generator.h"

#include <algorithm>
#include <cstdint>
#include <functional>
#include <regex>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/executable/data/electron_app_info.h"
#include "binary_valentine/rule_class.h"

#include "pe_bliss2/exports/export_directory.h"
#include "pe_bliss2/image/image.h"
#include "pe_bliss2/image/section_data_from_va.h"
#include "pe_bliss2/packed_c_string.h"
#include "pe_bliss2/packed_struct.h"

namespace bv::pe
{

class electron_app_info_generator final
	: public core::data_generator_base<electron_app_info_generator>
{
private:
	static constexpr std::string_view v8_version_string_symbol = "?version_string_@Version@internal@v8";

public:
	static constexpr std::string_view generator_name = "pe_electron_app_info_generator";

	[[nodiscard]]
	core::typed_value_ptr<executable::electron_app_info> generate(
		const pe_bliss::image::image& image,
		const pe_bliss::exports::export_directory_details& exports) const
	{
		if (auto v8_version_str = get_v8_version_string(image, exports); v8_version_str)
		{
			auto suffix_pos = v8_version_str->find("-electron.");
			if (suffix_pos == std::string::npos)
				return nullptr;

			auto result = core::make_value<executable::electron_app_info>();
			result->get_value().v8_version_string = v8_version_str->substr(0, suffix_pos);

			try
			{
				search_for_electron_version(image, result->get_value().electron_version);
			}
			catch (const std::exception&)
			{
				result->get_value().electron_version.reset();
			}

			return result;
		}

		return nullptr;
	}

private:
	template<typename T>
	static T get_packed_int(
		const pe_bliss::image::image& image, pe_bliss::rva_type rva)
	{
		pe_bliss::packed_struct<T> value;
		const auto buf = pe_bliss::image::section_data_from_rva(
			image, rva, pe_bliss::packed_struct<T>::packed_size, false, false);
		buffers::input_buffer_stateful_wrapper_ref wrapper(*buf);
		value.deserialize(wrapper, false);
		return value.get();
	}

	static std::optional<std::string> get_v8_version_string(
		const pe_bliss::image::image& image,
		const pe_bliss::exports::export_directory_details& exports)
	{
		std::optional<std::string> result;
		try
		{
			const auto rva = get_v8_version_string_symbol_rva(exports);
			if (!rva)
				return result;

			std::uint64_t version_string_va{};
			if (image.is_64bit())
				version_string_va = get_packed_int<std::uint64_t>(image, rva);
			else
				version_string_va = get_packed_int<std::uint32_t>(image, rva);

			pe_bliss::packed_c_string version_string;
			const auto buf = pe_bliss::image::section_data_from_va(image, version_string_va);
			buffers::input_buffer_stateful_wrapper_ref wrapper(*buf);
			version_string.deserialize(wrapper, false);
			result.emplace(std::move(version_string).value());
		}
		catch (const std::exception&)
		{
			// Return empty result
		}

		return result;
	}

	static pe_bliss::rva_type get_v8_version_string_symbol_rva(
		const pe_bliss::exports::export_directory_details& exports)
	{
		//TODO: when exports search is needed in some other rule/generator,
		// put exports to a hash map in a separate data generator
		for (const auto& symbol : exports.get_export_list())
		{
			if (symbol.get_names().size() == 1u)
			{
				const auto& name = symbol.get_names()[0].get_name();
				if (name && std::string_view(name->value())
					.starts_with(v8_version_string_symbol))
				{
					return symbol.get_rva().get();
				}
			}
		}
		return {};
	}

	static const pe_bliss::section::section_data* find_rdata_section(
		const pe_bliss::image::image& image)
	{
		std::size_t index = 0;
		for (const auto& header : image.get_section_table().get_section_headers())
		{
			if (header.get_name() == ".rdata")
				return &image.get_section_data_list().at(index);

			++index;
		}

		return nullptr;
	}

	static void search_for_electron_version(std::span<const std::byte> rdata,
		std::optional<executable::electron_version_info>& electron_version)
	{
		static constexpr std::array target_string{
			std::byte{' '},
			std::byte{'E'},
			std::byte{'l'},
			std::byte{'e'},
			std::byte{'c'},
			std::byte{'t'},
			std::byte{'r'},
			std::byte{'o'},
			std::byte{'n'},
			std::byte{'/'}
		};
		static const std::boyer_moore_horspool_searcher searcher(
			target_string.begin(), target_string.end());
		static constexpr std::size_t max_tries = 3u;
		static constexpr std::size_t max_length = 30u;

		//Example versions:
		//4.0.0-beta.8
		//18.3.5
		static const std::regex version_regex{ "(\\d+)\\.(\\d+)\\.(\\d+)(?:\\-(alpha|beta)\\.(\\d+))?",
			std::regex_constants::ECMAScript | std::regex_constants::optimize };

		auto from = rdata.begin();
		const auto to = rdata.end();
		std::string candidate;
		for (std::size_t tries = 0; tries < max_tries; ++tries)
		{
			from = std::search(from, to, searcher);
			if (from == to)
				return;

			from += target_string.size();
			candidate.clear();
			while (from != to && *from != std::byte{} && candidate.size() < max_length)
			{
				candidate.push_back(static_cast<char>(*from++));
			}

			std::smatch match;
			if (std::regex_match(candidate, match, version_regex))
			{
				auto& ver = electron_version.emplace();
				ver.major = boost::lexical_cast<std::uint32_t>(match[1].str());
				ver.minor = boost::lexical_cast<std::uint32_t>(match[2].str());
				ver.build = boost::lexical_cast<std::uint32_t>(match[3].str());

				if (match[5].matched)
				{
					ver.pre_release_version = boost::lexical_cast<std::uint32_t>(match[5].str());
					ver.build_type = match[4] == "alpha"
						? executable::electron_build_type::alpha
						: executable::electron_build_type::beta;
				}
				ver.full_string = std::move(candidate);
				return;
			}
		}
	}

	static void search_for_electron_version(
		const pe_bliss::image::image& image,
		std::optional<executable::electron_version_info>& electron_version)
	{
		const auto* rdata = find_rdata_section(image);
		if (!rdata)
			return;

		const std::size_t size = rdata->data()->size();
		if (rdata->data()->is_stateless())
		{
			const std::byte* raw_data = rdata->data()->get_raw_data(
				0, rdata->data()->physical_size());
			search_for_electron_version(std::span<const std::byte>(raw_data, size),
				electron_version);
			return;
		}

		static constexpr std::size_t max_rdata_size = 1024 * 1024 * 100; //100 MB
		if (size > max_rdata_size)
			return;

		//TODO: move this to a separate generator once copied rdata is needed
		//somewhere else
		std::vector<std::byte> copied_data;
		copied_data.resize(size);
		rdata->data()->read(0, size, copied_data.data());
		search_for_electron_version(copied_data, electron_version);
	}
};

void electron_app_info_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<electron_app_info_generator>();
}

} //namespace bv::pe
