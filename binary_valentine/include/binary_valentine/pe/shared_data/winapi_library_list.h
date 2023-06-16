#pragma once

#include <deque>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <unordered_map>
#include <utility>

#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/pe/shared_data/api_sets.h"
#include "binary_valentine/string/case_conv.h"

namespace bv::pe
{

class [[nodiscard]] winapi_entry final
{
public:
	template<typename Name>
	explicit winapi_entry(Name&& name)
		: name_(std::forward<Name>(name))
	{
		if (name_.empty())
			throw std::runtime_error("Empty API name");
	}
	
	[[nodiscard]]
	const std::string& get_name() const noexcept
	{
		return name_;
	}

private:
	std::string name_;
};

template<typename ApiEntry>
class [[nodiscard]] winapi_library final
{
public:
	using api_entry_type = ApiEntry;

public:
	winapi_library() = default;
	winapi_library(const winapi_library&) = delete;
	winapi_library& operator=(const winapi_library&) = delete;
	winapi_library(winapi_library&&) = delete;
	winapi_library& operator=(winapi_library&&) = delete;

	bool try_add(const api_entry_type& api)
	{
		auto existing = map_.try_emplace(api.get_name(), api).first->second;
		return &existing.get() == &api;
	}

	const api_entry_type* get_api(std::string_view func_name) const
	{
		if (auto it = map_.find(func_name); it != map_.end())
			return &it->second.get();

		return nullptr;
	}

private:
	using name_to_api_map_type = std::unordered_map<std::string_view,
		std::reference_wrapper<const api_entry_type>,
		core::transparent_hash, std::equal_to<>>;

	name_to_api_map_type map_;
};

template<typename WinapiLibrary>
class [[nodiscard]] winapi_library_map final
{
public:
	using winapi_library_type = WinapiLibrary;
	using api_entry_type = typename winapi_library_type::api_entry_type;

	using dll_to_apis_map_type = std::unordered_map<std::string,
		winapi_library_type, core::transparent_hash, std::equal_to<>>;

public:
	[[nodiscard]]
	const winapi_library_type* get_library(std::string_view dll) const
	{
		auto dll_lowercase = string::to_lower_ascii(dll);
		std::string_view dll_lowercase_view(dll_lowercase);
		reduce_api_set_name(dll_lowercase_view);
		if (auto dll_it = dll_to_api_.find(dll_lowercase_view);
			dll_it != dll_to_api_.end())
		{
			return &dll_it->second;
		}

		return nullptr;
	}

	template<typename Name, typename... Args>
	api_entry_type& add_api(Name&& name, Args&&... args)
	{
		return apis_.emplace_back(std::forward<Name>(name), 
			std::forward<Args>(args)...);
	}

	void add_dll(const api_entry_type& api, std::string_view dll_name)
	{
		reduce_api_set_name(dll_name);
		auto api_it = dll_to_api_.emplace(std::piecewise_construct,
			std::forward_as_tuple(dll_name), std::forward_as_tuple()).first;
		if (!api_it->second.try_add(api))
			throw std::runtime_error("Duplicate API " + api.get_name());
	}

	void try_add_api_set(const api_entry_type& api, std::string_view api_set_name)
	{
		auto api_it = dll_to_api_.emplace(std::piecewise_construct,
			std::forward_as_tuple(api_set_name), std::forward_as_tuple()).first;
		api_it->second.try_add(api);
	}

private:
	std::deque<api_entry_type> apis_;
	dll_to_apis_map_type dll_to_api_;
};

using winapi_library_type = winapi_library<winapi_entry>;
using full_winapi_library_map_type = winapi_library_map<winapi_library_type>;

} //namespace bv::pe
