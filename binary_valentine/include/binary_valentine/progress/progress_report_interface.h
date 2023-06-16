#pragma once

#include <memory>
#include <string_view>

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::progress
{

enum class progress_state
{
	load_started,
	loaded,
	analysis_started,
	analysis_completed,
	combined_analysis_started,
	combined_analysis_completed
};

[[nodiscard]]
std::string_view progress_state_to_string(progress_state state) noexcept;

class [[nodiscard]] progress_report_interface
{
public:
	virtual ~progress_report_interface() = default;

	virtual void report_progress(
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		progress_state state) noexcept = 0;
};

} //namespace bv::progress
