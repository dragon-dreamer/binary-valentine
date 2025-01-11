#pragma once

#include <memory>
#include <optional>

#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_interface.h"

namespace bv::core
{

class [[nodiscard]] value_provider_interface_base
{
public:
	virtual ~value_provider_interface_base() = default;
	virtual void set(value_tag tag, value_ptr value) = 0;
	[[nodiscard]]
	virtual std::optional<const value_interface*> try_get(value_tag tag) const = 0;
	virtual bool remove(value_tag tag) = 0;

	template<typename T>
	void set(std::unique_ptr<T> value)
	{
		set(T::get_tag(), std::move(value));
	}

	template<typename T>
	[[nodiscard]]
	const T* try_get() const
	{
		auto result = try_get(value<T>::get_tag());
		if (!result || !*result)
			return nullptr;

		return &static_cast<const value<T>&>(**result).get_value();
	}
};

class [[nodiscard]] value_provider_interface : public value_provider_interface_base
{
public:
	virtual const value_interface* get(value_tag tag) = 0;

	template<typename T>
	[[nodiscard]]
	const T* get()
	{
		const auto* result = get(value<T>::get_tag());
		if (!result)
			return nullptr;

		return &static_cast<const value<T>&>(*result).get_value();
	}
};

} //namespace bv::core
