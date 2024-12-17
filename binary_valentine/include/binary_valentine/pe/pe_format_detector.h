#pragma once

#include <string>
#include <unordered_set>

#include "binary_valentine/core/rule_detector_interface.h"
#include "binary_valentine/rule_class.h"

namespace bv::pe
{

class [[nodiscard]] pe_format_detector final
	: public core::rule_detector_interface
{
public:
	pe_format_detector() = default;

	virtual boost::asio::awaitable<bool> detect(
		const core::subject_entity_interface& entity,
		core::entity_stream_provider_interface& stream_provider,
		core::value_cache& values,
		core::rule_class_mask& rules) const override;

private:
	std::unordered_set<std::string> executable_extensions_{
		".exe", ".dll", ".efi", ".ocx", ".scr", ".sys"
	};
	//TODO: make configurable?
	std::size_t max_in_memory_size = 250 * 1024 * 1024; // 250 MB
};

} //namespace bv::pe
