#pragma once

#include <vector>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/rule_class.h"

namespace bv::core
{

class subject_entity_interface;
class value_cache;
class entity_stream_provider_interface;

class [[nodiscard]] rule_detector_interface
{
public:
	virtual ~rule_detector_interface() = default;

	[[nodiscard]]
	virtual boost::asio::awaitable<bool> detect(
		const subject_entity_interface& entity,
		entity_stream_provider_interface& stream_provider,
		value_cache& values,
		std::vector<core::rule_class_type>& rules) const = 0;
};

} //namespace bv::core
