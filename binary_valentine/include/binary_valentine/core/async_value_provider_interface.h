#pragma once

#include <memory>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/value_provider_interface.h"

namespace bv::core
{

class [[nodiscard]] async_value_provider_interface : public value_provider_interface_base
{
public:
	[[nodiscard]]
	virtual boost::asio::awaitable<const value_interface*> get_async(value_tag tag) = 0;

	[[nodiscard]]
	virtual value_provider_interface& get_sync_provider() noexcept = 0;

	template<typename T>
	[[nodiscard]]
	boost::asio::awaitable<const T*> get_async()
	{
		const auto* result = co_await get_async(value<T>::get_tag());
		if (!result)
			co_return nullptr;

		co_return &static_cast<const value<T>&>(*result).get_value();
	}
};

} //namespace bv::core
