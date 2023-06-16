#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::string
{

class [[nodiscard]] embedded_resource_provider final : public resource_provider_interface
{
public:
	template<typename Language, typename... Args>
	explicit embedded_resource_provider(Language&& language, Args&&... args)
		: language_(std::forward<Language>(language))
		, strings_(std::forward<Args>(args)...)
	{
	}

	[[nodiscard]]
	virtual std::string_view get_string(std::string_view key) const override
	{
		auto it = strings_.find(key);
		if (it == strings_.end())
			return key;

		return it->second;
	}

	[[nodiscard]]
	std::size_t size() const noexcept
	{
		return strings_.size();
	}

	[[nodiscard]]
	virtual std::string_view get_language() const noexcept override
	{
		return language_;
	}

	template<typename Key, typename Value>
	bool add_string(Key&& key, Value&& value)
	{
		return strings_.emplace(std::forward<Key>(key),
			std::forward<Value>(value)).second;
	}

private:
	std::string language_;
	std::unordered_map<std::string_view, std::string_view,
		core::transparent_hash, std::equal_to<>> strings_;
};

} //namespace bv::string
