#pragma once

#include <chrono>
#include <cstdint>

namespace bv::output
{
class entity_in_memory_report_interface;
} //namespace bv::output

namespace bv::core
{
class time_tracker;
} //namespace bv::core

namespace bv::output::format
{

struct analysis_state
{
	std::chrono::utc_clock::time_point start;
	std::chrono::utc_clock::time_point end;
	std::chrono::milliseconds duration;
	std::uint32_t analysis_issues{};
	std::uint32_t entity_issues{};
	bool interrupted{};
};

[[nodiscard]]
analysis_state state_from_time_tracker(const core::time_tracker& tracker) noexcept;

class [[nodiscard]] output_format_interface
{
public:
	virtual ~output_format_interface() = default;

	virtual void start(const analysis_state& state) = 0;
	virtual void append(const entity_in_memory_report_interface& report) = 0;
	virtual void finalize() = 0;
};

} //namespace bv::output::format
