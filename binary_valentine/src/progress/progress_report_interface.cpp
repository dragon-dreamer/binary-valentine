#include "binary_valentine/progress/progress_report_interface.h"

#include <cassert>

#include "binary_valentine/output/internal_report_messages.h"

namespace bv::progress
{

std::string_view progress_state_to_string(progress_state state) noexcept
{
	switch (state)
	{
	case progress_state::analysis_completed:
		return output::reports::progress_analysis_completed;
	case progress_state::analysis_started:
		return output::reports::progress_analysis_started;
	case progress_state::loaded:
		return output::reports::progress_loaded;
	case progress_state::load_started:
		return output::reports::progress_load_started;
	case progress_state::combined_analysis_started:
		return output::reports::progress_combined_analysis_started;
	case progress_state::combined_analysis_completed:
		return output::reports::progress_combined_analysis_completed;
	case progress_state::target_skipped_unsupported:
		return output::reports::progress_target_skipped_unsupported;
	case progress_state::target_skipped_filtered:
		return output::reports::progress_target_skipped_filtered;
	case progress_state::shared_data_load_started:
		return output::reports::progress_shared_data_load_started;
	case progress_state::shared_data_load_completed:
		return output::reports::progress_shared_data_load_completed;
	default:
		assert(false);
		return {};
	}
}

} //namespace bv::progress
