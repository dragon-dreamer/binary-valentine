#include "binary_valentine/pe/generator/manifest_generator.h"

#include <array>
#include <cassert>
#include <memory>
#include <exception>
#include <filesystem>
#include <span>
#include <string_view>
#include <system_error>
#include <utility>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/async_data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/file/async_file.h"
#include "binary_valentine/pe/data/basic_pe_info.h"
#include "binary_valentine/pe/data/manifest_info.h"

#include "buffers/input_buffer_interface.h"
#include "buffers/input_container_buffer.h"
#include "buffers/ref_buffer.h"

#include "pe_bliss2/resources/manifest.h"
#include "pe_bliss2/resources/pugixml_manifest_accessor.h"
#include "pe_bliss2/resources/resource_types.h"
#include "pe_bliss2/resources/resource_directory.h"
#include "pe_bliss2/resources/resource_reader.h"

namespace bv::pe
{

class manifest_generator final
	: public core::async_data_generator_base<manifest_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_manifest_generator";

	boost::asio::awaitable<bool> can_generate(const basic_pe_info& info) const
	{
		co_return !info.is_boot && !info.is_pre_win7ce;
	}

	[[nodiscard]]
	boost::asio::awaitable<core::typed_value_ptr<manifest_info>> generate(
		const basic_pe_info& info,
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		const pe_bliss::resources::resource_directory_details* root) const
	{
		const auto type = info.file_extension == ".exe"
			? manifest_type::application
			: manifest_type::assembly;

		auto result = core::make_value<manifest_info>();
		auto& manifest_data = result->get_value();

		const auto resource_id = type == manifest_type::application
			? pe_bliss::resources::manifest_resource_id::createprocess
			: pe_bliss::resources::manifest_resource_id::isolationaware;

		if (try_load_manifest_from_image(manifest_data, root, resource_id))
		{
			manifest_data.type = type;

			for (const auto& file_name : get_manifest_file_names(entity->get_path(), type))
			{
				std::error_code ec;
				if (std::filesystem::exists(file_name, ec))
				{
					manifest_data.external_result = external_manifest_load_result::exists;
					break;
				}
			}
		}
		else
		{
			if (co_await try_load_manifest_file(get_manifest_file_names(entity->get_path(), type),
				manifest_data))
			{
				manifest_data.type = type;
			}
		}

		co_return result;
	}

private:
	static std::array<std::filesystem::path, 2>
		get_manifest_file_names(const std::filesystem::path& binary_name,
		manifest_type type)
	{
		std::array<std::filesystem::path, 2> result{
			binary_name, binary_name
		};
		result[0].concat(".manifest");
		if (type == manifest_type::application)
		{
			//resource ID for app manifests is 1 (CREATEPROCESS_MANIFEST_RESOURCE_ID)
			result[1].concat(".1.manifest");
		}
		else
		{
			//resource ID for assembly manifests is 2 (ISOLATIONAWARE_MANIFEST_RESOURCE_ID)
			result[1].concat(".2.manifest");
		}
		return result;
	}

	static boost::asio::awaitable<bool> try_load_manifest_file(
		std::span<const std::filesystem::path> manifest_paths,
		manifest_info& data)
	{
		assert(!manifest_paths.empty());
		auto buf = std::make_shared<buffers::input_container_buffer>();
		file::async_file_read_result read_result{};
		for (const auto& file_name : manifest_paths)
		{
			try
			{
				read_result = co_await file::async_file::read_async(
					file_name, buf->get_container(), max_manifest_file_size);
				if (read_result != file::async_file_read_result::does_not_exist)
					break;
			}
			catch (const std::exception&)
			{
				data.manifest_load_error = std::current_exception();
				data.external_result = external_manifest_load_result::error;
				co_return true;
			}
		}

		if (read_result == file::async_file_read_result::does_not_exist)
		{
			data.external_result = external_manifest_load_result::does_not_exist;
			co_return false;
		}

		if (read_result == file::async_file_read_result::too_big)
		{
			data.external_result = external_manifest_load_result::too_big;
			co_return true;
		}

		// read_result == ok
		load_manifest(buf, data);
		if (data.manifest_load_error)
			data.external_result = external_manifest_load_result::error;
		else
			data.external_result = external_manifest_load_result::loaded;

		co_return true;
	}

	static bool try_load_manifest_from_image(manifest_info& data,
		const pe_bliss::resources::resource_directory_details* root,
		pe_bliss::resources::manifest_resource_id resource_id)
	{
		if (!root)
			return false;

		const buffers::ref_buffer* manifest_buf{};

		try
		{
			manifest_buf = &pe_bliss::resources::get_resource_data_by_id(*root, 0u,
				pe_bliss::resources::resource_type::manifest,
				static_cast<pe_bliss::resources::resource_id_type>(resource_id));
		}
		catch (const std::system_error&)
		{
			return false;
		}

		data.has_embedded_manifest = true;
		load_manifest(manifest_buf->data(), data);
		return true;
	}

	static void load_manifest(const buffers::input_buffer_ptr& buf, manifest_info& data)
	{
		try
		{
			auto manifest_accessor = pe_bliss::resources::pugixml::parse_manifest(buf);
			if (!manifest_accessor->get_root())
			{
				throw std::system_error(manifest_accessor
					->get_errors().get_errors()->begin()->first.code);
			}
			
			data.manifest = pe_bliss::resources::parse_manifest(*manifest_accessor);
			if (manifest_accessor->get_errors().has_errors())
			{
				for (const auto& [ctx, err] : *manifest_accessor->get_errors().get_errors())
					data.manifest->add_error(ctx.code);
			}
		}
		catch (const std::system_error&)
		{
			data.manifest.reset();
			data.manifest_load_error = std::current_exception();
		}
	}

private:
	static constexpr std::size_t max_manifest_file_size = 1024 * 50; //50 Kb
};

void manifest_generator_factory::add_generator(core::async_data_generator_list& generators)
{
	generators.add<manifest_generator>();
}

} //namespace bv::pe
