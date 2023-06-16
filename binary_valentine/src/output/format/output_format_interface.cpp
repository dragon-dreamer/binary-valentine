#include "binary_valentine/output/format/output_format_interface.h"

#include "binary_valentine/core/time_tracker.h"

namespace bv::output::format
{

analysis_state state_from_time_tracker(const core::time_tracker& tracker) noexcept
{
	return {
		.start = tracker.get_start(),
		.end = tracker.get_end(),
		.duration = tracker.get_duration(),
		.interrupted = tracker.is_interrupted()
	};
}

} //namespace bv::output::format
