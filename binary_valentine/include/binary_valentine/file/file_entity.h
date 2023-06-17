#pragma once

#include <filesystem>
#include <utility>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/file/file_entity_stream_provider.h"
#include "binary_valentine/string/encoding.h"

namespace bv::file
{

class [[nodiscard]] file_entity final : public core::subject_entity_interface
{
public:
	template<typename T>
	explicit file_entity(T&& entry)
		: entry_(std::forward<T>(entry))
		, name_(name_from_path(entry_.path()))
		, utf8_path_(string::to_utf8(entry_.path().native()))
	{
	}

	template<typename T>
	explicit file_entity(T&& entry, const std::filesystem::path& root_path)
		: entry_(std::forward<T>(entry))
		, name_(name_from_path(entry_.path(), root_path))
		, utf8_path_(string::to_utf8(entry_.path().native()))
	{
	}

	[[nodiscard]]
	virtual bool is_directory() const override
	{
		return entry_.is_directory();
	}

	[[nodiscard]]
	virtual bool is_regular_file() const override
	{
		return entry_.is_regular_file();
	}

	[[nodiscard]]
	virtual std::filesystem::file_status get_status() const override
	{
		return entry_.status();
	}

	[[nodiscard]]
	virtual const std::uint64_t get_size() const override
	{
		return entry_.file_size();
	}

	[[nodiscard]]
	virtual const std::filesystem::path& get_path() const noexcept override
	{
		return entry_.path();
	}

	[[nodiscard]]
	virtual std::string_view get_utf8_path() const noexcept override
	{
		return utf8_path_;
	}

	[[nodiscard]]
	virtual std::unique_ptr<core::entity_stream_provider_interface>
		get_stream_provider() const override
	{
		return std::make_unique<file_entity_stream_provider>(entry_.path());
	}

	[[nodiscard]]
	virtual std::string_view get_name() const noexcept override
	{
		return name_;
	}

	[[nodiscard]]
	virtual std::filesystem::file_time_type get_last_write_time() const override
	{
		return entry_.last_write_time();
	}

private:
	std::filesystem::directory_entry entry_;
	std::string name_;
	std::string utf8_path_;
};

} //namespace bv::file
