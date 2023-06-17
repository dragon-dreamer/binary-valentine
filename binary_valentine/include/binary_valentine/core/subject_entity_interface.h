#pragma once

#include <filesystem>
#include <memory>
#include <string>

#include "binary_valentine/core/entity_stream_provider_interface.h"

namespace bv::core
{

class [[nodiscard]] subject_entity_interface
{
public:
	virtual ~subject_entity_interface() = default;
	
	[[nodiscard]]
	virtual bool is_regular_file() const = 0;
	[[nodiscard]]
	virtual bool is_directory() const = 0;
	[[nodiscard]]
	virtual std::filesystem::file_status get_status() const = 0;
	[[nodiscard]]
	virtual const std::filesystem::path& get_path() const noexcept = 0;
	[[nodiscard]]
	virtual std::string_view get_utf8_path() const noexcept = 0;
	[[nodiscard]]
	virtual std::string_view get_name() const noexcept = 0;
	[[nodiscard]]
	virtual std::uint64_t get_size() const = 0;
	[[nodiscard]]
	virtual std::filesystem::file_time_type get_last_write_time() const = 0;

	[[nodiscard]]
	virtual std::unique_ptr<entity_stream_provider_interface> get_stream_provider() const = 0;

protected:
	[[nodiscard]]
	static std::string name_from_path(const std::filesystem::path& path);
	[[nodiscard]]
	static std::string name_from_path(const std::filesystem::path& path,
		const std::filesystem::path& root_path);
};

} //namespace bv::core
