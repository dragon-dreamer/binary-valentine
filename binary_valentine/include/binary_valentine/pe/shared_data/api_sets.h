#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "binary_valentine/core/transparent_hash.h"

namespace bv::pe
{

[[nodiscard]]
bool is_api_set_name(std::string_view dll_name);
bool reduce_api_set_name(std::string_view& dll_name);

class [[nodiscard]] api_sets final
{
public:
	using dll_to_dll_list_type = std::unordered_map<std::string,
		std::vector<std::string>, core::transparent_hash, std::equal_to<>>;

public:
	template<typename ApiSetName, typename DllName>
	void add_api_set(ApiSetName&& api_set_name, DllName&& dll_name)
	{
		std::string_view api_set_name_view(api_set_name);
		reduce_api_set_name(api_set_name_view);
		auto it = api_set_to_dlls_.find(api_set_name_view);
		if (it == api_set_to_dlls_.end())
		{
			api_set_to_dlls_[std::string(api_set_name_view)]
				.emplace_back(std::forward<DllName>(dll_name));
		}
		else
		{
			it->second.emplace_back(std::forward<DllName>(dll_name));
		}
	}

	void build_dll_to_api_set_map()
	{
		for (const auto& [api_set, dlls] : api_set_to_dlls_)
		{
			for (const auto& dll : dlls)
				dll_to_api_sets_[dll].emplace_back(api_set);
		}
	}

	[[nodiscard]]
	const std::vector<std::string>* get_api_sets_for_dll(std::string_view dll) const
	{
		auto it = dll_to_api_sets_.find(dll);
		if (it == dll_to_api_sets_.end())
			return nullptr;

		return &it->second;
	}

private:
	dll_to_dll_list_type api_set_to_dlls_;
	dll_to_dll_list_type dll_to_api_sets_;
};

} //namespace bv::pe
