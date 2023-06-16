#include "binary_valentine/core/rule_detector_container.h"

#include <cassert>
#include <functional>
#include <utility>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/core/value_cache.h"

namespace bv::core
{

boost::asio::awaitable<bool> rule_detector_container::detect(
	const subject_entity_interface& entity,
	entity_stream_provider_interface& stream_provider,
	value_cache& values,
	std::vector<core::rule_class_type>& rules) const
{
	bool file_format_detected = false;
	for (const auto& detector : file_format_detectors_)
	{
		if (co_await detector->detect(entity, stream_provider, values, rules))
		{
			file_format_detected = true;
			break;
		}
	}

	if (!file_format_detected)
		co_return false;

	for (const auto& detector : extra_detectors_)
		co_await detector->detect(entity, stream_provider, values, rules);

	co_return true;
}

void rule_detector_container::add_file_format_detector(std::unique_ptr<
	const rule_detector_interface>&& detector)
{
	assert(!!detector);
	file_format_detectors_.emplace_back(std::move(detector));
}

void rule_detector_container::add_extra_detector(std::unique_ptr<
	const rule_detector_interface>&& detector)
{
	assert(!!detector);
	extra_detectors_.emplace_back(std::move(detector));
}

} //namespace bv::core
