#include "binary_valentine/core/value_cache.h"

namespace bv::core
{

value_cache_result value_cache::get(value_tag tag) const
{
	auto it = values_.find(tag);
	if (it == values_.end())
		return {};

	return { true, it->second.get() };
}

value_cache_result value_cache::set(value_tag tag, value_ptr&& val)
{
	auto& stored = values_[tag];
	stored = std::move(val);

	return { true, stored.get() };
}

bool value_cache::remove(value_tag tag)
{
	return values_.erase(tag) > 0u;
}

} //namespace bv::core
