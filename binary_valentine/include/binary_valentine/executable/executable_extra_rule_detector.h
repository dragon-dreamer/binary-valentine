#pragma once

#include "binary_valentine/core/rule_detector_interface.h"

namespace bv::core
{
class entity_stream_provider_interface;
class value_cache;
} //namespace bv::core

namespace bv::executable
{

class [[nodiscard]] executable_extra_rule_detector final
	: public core::rule_detector_interface
{
public:
	virtual ~executable_extra_rule_detector() = default;

	virtual boost::asio::awaitable<bool> detect(
		const core::subject_entity_interface& entity,
		core::entity_stream_provider_interface& /* stream_provider */,
		core::value_cache& values,
		std::vector<core::rule_class_type>& rules) const override;
};

} //namespace bv::executable
