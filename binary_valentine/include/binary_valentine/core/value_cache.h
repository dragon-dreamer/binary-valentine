#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "binary_valentine/core/value_interface.h"

namespace bv::core
{

struct value_cache_result
{
	bool initialized{};
	const value_interface* value{};
};

class [[nodiscard]] value_cache final
{
public:
	[[nodiscard]]
	value_cache_result get(value_tag tag) const;
	value_cache_result set(value_tag tag, value_ptr&& val);
	bool remove(value_tag tag);
	bool empty() const noexcept
	{
		return values_.empty();
	}

	template<typename T>
	void set(std::unique_ptr<T> value)
	{
		set(T::get_tag(), std::move(value));
	}
	
	template<typename Func>
	void remove_if(Func&& func)
	{
		for (auto it = values_.begin(); it != values_.end(); )
		{
			if (func(it->first))
				it = values_.erase(it);
			else
				++it;
		}
	}

private:
	std::unordered_map<value_tag, value_ptr> values_;
};

} //namespace bv::core
