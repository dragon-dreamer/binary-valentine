#pragma once

#include <chrono>

namespace bv::core
{

class [[nodiscard]] time_tracker final
{
public:
	time_tracker() = default;
	time_tracker(const time_tracker&) = delete;
	time_tracker(time_tracker&&) = delete;
	time_tracker& operator=(const time_tracker&) = delete;
	time_tracker& operator=(time_tracker&&) = delete;

	void start();
	void stop(bool interrupt);

	[[nodiscard]]
	const std::chrono::milliseconds& get_duration() const noexcept
	{
		return duration_;
	}

	[[nodiscard]]
	const std::chrono::utc_clock::time_point& get_start() const noexcept
	{
		return start_;
	}

	[[nodiscard]]
	const std::chrono::utc_clock::time_point& get_end() const noexcept
	{
		return end_;
	}

	[[nodiscard]]
	bool is_interrupted() const noexcept
	{
		return is_interrupted_;
	}

private:
	std::chrono::utc_clock::time_point start_;
	std::chrono::utc_clock::time_point end_;
	std::chrono::steady_clock::time_point steady_start_;
	std::chrono::milliseconds duration_;
	bool started_{};
	bool is_interrupted_{};
};

} //namespace bv::core
