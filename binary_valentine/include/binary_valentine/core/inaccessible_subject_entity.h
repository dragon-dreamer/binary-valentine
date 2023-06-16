#pragma once

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/string/encoding.h"

namespace bv::core
{

class [[nodiscard]] inaccessible_subject_entity final
	: public subject_entity_interface
{
public:
	template<typename T>
	explicit inaccessible_subject_entity(T&& path)
		: path_(std::forward<T>(path))
		, name_(name_from_path(path_))
		, utf8_path_(string::to_utf8(path_.native()))
	{
	}

	template<typename T>
	explicit inaccessible_subject_entity(T&& path,
		const std::filesystem::path& root_path)
		: path_(std::forward<T>(path))
		, name_(name_from_path(path_, root_path))
		, utf8_path_(string::to_utf8(path_.native()))
	{
	}

	[[nodiscard]]
	virtual bool is_regular_file() const override
	{
		return false;
	}

	[[nodiscard]]
	virtual bool is_directory() const override
	{
		return false;
	}

	[[nodiscard]]
	virtual std::filesystem::file_status get_status() const override
	{
		return {};
	}

	[[nodiscard]]
	virtual const std::uint64_t get_size() const override
	{
		return 0u;
	}

	[[nodiscard]]
	virtual const std::filesystem::path& get_path() const noexcept override
	{
		return path_;
	}

	[[nodiscard]]
	virtual std::string_view get_utf8_path() const noexcept override
	{
		return utf8_path_;
	}

	[[nodiscard]]
	virtual std::string_view get_name() const noexcept override
	{
		return name_;
	}

	[[nodiscard]]
	virtual std::unique_ptr<entity_stream_provider_interface> get_stream_provider() const override
	{
		throw std::runtime_error("Inaccessible entity");
	}

	[[nodiscard]]
	virtual std::filesystem::file_time_type get_last_write_time() const override
	{
		throw std::runtime_error("Inaccessible entity");
	}

private:
	std::filesystem::path path_;
	std::string name_;
	std::string utf8_path_;
};

} //namespace bv::core
