#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include "binary_valentine/pe/shared_data/winapi_library_list.h"

namespace bv::pe
{

struct api_flags final
{
	api_flags() = delete;
	enum value
	{
		deprecated = 1 << 0,
		dangerous = 1 << 1,
		insecure = 1 << 2,
		internal = 1 << 3,
		xbox_only = 1 << 4
	};
};

class [[nodiscard]] not_recommended_api final
{
public:
	template<typename Name, typename Mitigation>
	explicit not_recommended_api(Name&& name, api_flags::value flags,
		Mitigation&& mitigation)
		: name_(std::forward<Name>(name))
		, mitigation_(std::forward<Mitigation>(mitigation))
		, flags_(flags)
	{
		if (name_.empty())
			throw std::runtime_error("Empty API name");
		if (mitigation_.empty())
			throw std::runtime_error("Empty mitigation for " + name_);
		if (flags_ == 0u)
			throw std::runtime_error("Empty flags for API " + name_);
	}

	template<typename Name>
	void add_min_os_version(Name&& name)
	{
		min_os_versions_.emplace_back(std::forward<Name>(name));
	}

	[[nodiscard]]
	const std::string& get_name() const noexcept
	{
		return name_;
	}

	[[nodiscard]]
	api_flags::value get_flags() const noexcept
	{
		return flags_;
	}

	[[nodiscard]]
	const std::string& get_mitigation() const noexcept
	{
		return mitigation_;
	}

private:
	std::string name_;
	std::vector<std::string> min_os_versions_; //TODO: optimize and use IDs instead of strings
	std::string mitigation_;
	api_flags::value flags_{};
};

using not_recommended_api_map = winapi_library<not_recommended_api>;
using not_recommended_imports = winapi_library_map<not_recommended_api_map>;

} //namespace bv::pe
