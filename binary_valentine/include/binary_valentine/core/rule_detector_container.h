#pragma once

#include <memory>
#include <vector>

#include "binary_valentine/core/rule_class.h"
#include "binary_valentine/core/rule_detector_interface.h"

namespace bv::core
{

class value_cache;
class entity_stream_provider_interface;
class subject_entity_interface;

class [[nodiscard]] rule_detector_container final : public rule_detector_interface
{
public:
	virtual boost::asio::awaitable<bool> detect(
		const subject_entity_interface& entity,
		entity_stream_provider_interface& stream_provider,
		value_cache& values,
		std::vector<core::rule_class_type>& rules) const override;

	void add_file_format_detector(std::unique_ptr<
		const rule_detector_interface>&& detector);
	void add_extra_detector(std::unique_ptr<
		const rule_detector_interface>&& detector);

private:
	std::vector<std::unique_ptr<
		const rule_detector_interface>> file_format_detectors_;
	std::vector<std::unique_ptr<
		const rule_detector_interface>> extra_detectors_;
};

} //namespace bv::core
