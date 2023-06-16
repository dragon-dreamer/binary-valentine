#include "binary_valentine/core/time_tracker.h"

namespace bv::core
{

void time_tracker::start()
{
	if (started_)
		return;

	started_ = true;
	start_ = std::chrono::utc_clock::now();
	steady_start_ = std::chrono::steady_clock::now();
}

void time_tracker::stop(bool interrupt)
{
	if (!started_)
		return;

	started_ = false;
	is_interrupted_ = interrupt;
	end_ = std::chrono::utc_clock::now();
	duration_ = std::chrono::round<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - steady_start_);
}

} //namespace bv::core
