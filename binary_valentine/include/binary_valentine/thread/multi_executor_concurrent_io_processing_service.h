#pragma once

#include <atomic>
#include <cassert>
#include <csignal>
#include <cstddef>
#include <exception>
#include <utility>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/thread_pool.hpp>

#include "binary_valentine/core/time_tracker.h"
#include "binary_valentine/core/scoped_guard.h"

namespace bv::thread
{

/*
TaskImpl provides:
 * boost::asio::awaitable<void> io_task_impl();
 * boost::asio::awaitable<void> cpu_task_impl(io_result_type result);
 * void stop_requested();
*/
template<typename TaskImpl, typename IoResultType, typename TaskWeightType>
class [[nodiscard]] multi_executor_concurrent_io_processing_service
{
public:
	using io_result_type = IoResultType;
	using task_weight_type = TaskWeightType;
	using execution_context_type = boost::asio::thread_pool;

public:
	explicit multi_executor_concurrent_io_processing_service(
		std::size_t cpu_thread_count,
		task_weight_type max_task_weight)
		: max_task_weight_(max_task_weight)
		, pool_(cpu_thread_count)
		, io_pool_(1u)
		, cpu_task_complete_channel_(io_pool_)
	{
	}

	void join()
	{
		io_pool_.join();
		pool_.join();
		time_tracker_.stop(interrupted_);
	}

	void stop()
	{
		bool expected = false;
		if (interrupted_.compare_exchange_strong(expected, true))
		{
			static_cast<TaskImpl&>(*this).stop_requested();
			stop_cancellation_timers();
		}
	}

	[[nodiscard]]
	const core::time_tracker& get_time_tracker() const noexcept
	{
		return time_tracker_;
	}

	[[nodiscard]]
	execution_context_type& get_io_pool() noexcept
	{
		return io_pool_;
	}

	~multi_executor_concurrent_io_processing_service()
	{
		stop_cancellation_timers();
		join();
	}

protected:
	[[nodiscard]]
	boost::asio::awaitable<void> post_io_result_to_cpu(io_result_type result)
	{
		assert(io_pool_.get_executor().running_in_this_thread());

		auto weight = static_cast<TaskImpl&>(*this).get_task_weight(result);
		total_task_weight_ += weight;

		using namespace boost::asio::experimental::awaitable_operators;

		boost::asio::co_spawn(boost::asio::make_strand(pool_),
			(processing_task(std::move(result), std::move(weight)) || cpu_cancelled()),
			boost::asio::detached);

		// crashes on cancellation because of "||" race
		//boost::asio::co_spawn(pool_,
		//	(processing_task(std::move(result), std::move(weight)) || cpu_cancelled()),
		//	boost::asio::detached);
		
		while (total_task_weight_ >= max_task_weight_
			&& !(co_await boost::asio::this_coro::cancellation_state).cancelled())
		{
			auto [ec, task_weight] = co_await cpu_task_complete_channel_
				.async_receive(boost::asio::as_tuple(boost::asio::use_awaitable));
			if (ec)
				co_return;

			total_task_weight_ -= task_weight;
		}

		while (cpu_task_complete_channel_.try_receive([this](
			const boost::system::error_code&, auto&& task_weight) {
				total_task_weight_ -= task_weight;
			}));
	}

	void enable_signal_cancellation()
	{
		io_signal_set_.emplace(io_pool_, SIGINT, SIGTERM);
	}

	void start_after_preparation(
		boost::asio::awaitable<void> preparation)
	{
		time_tracker_.start();
		boost::asio::co_spawn(get_io_pool(),
			start_after_preparation_impl(std::move(preparation)),
			boost::asio::detached);
	}

private:
	boost::asio::awaitable<void> start_after_preparation_impl(
		boost::asio::awaitable<void> preparation)
	{
		if (io_signal_set_)
			boost::asio::co_spawn(io_pool_, io_signal_monitor(), boost::asio::detached);

		using namespace boost::asio::experimental::awaitable_operators;
		const auto res = co_await(std::move(preparation) || io_cancelled());
		if (res.index() == 0)
		{
			boost::asio::co_spawn(io_pool_, (io_task() || io_cancelled()), boost::asio::detached);
		}
	}

	boost::asio::awaitable<void> io_cancelled()
	{
		co_await cancelled(io_cancel_timer_);
	}

	void stop_cancellation_timers()
	{
		boost::asio::co_spawn(io_pool_, stop_cancellation_timers_coro(), boost::asio::detached);
	}

	boost::asio::awaitable<void> stop_cancellation_timers_coro()
	{
		stop_cancellation_timers_impl();
		co_return;
	}

	void stop_cancellation_timers_impl()
	{
		io_cancel_timer_.expires_at(std::chrono::steady_clock::time_point::min());
		cpu_cancellation_channel_.close();
		cpu_cancellation_channel_.cancel();
	}

	boost::asio::awaitable<void> io_signal_monitor()
	{
		using namespace boost::asio::experimental::awaitable_operators;

		assert(!!io_signal_set_);

		boost::system::error_code signal_ec;
		auto result = co_await (io_signal_set_->async_wait(
			boost::asio::redirect_error(boost::asio::use_awaitable, signal_ec)) || io_cancelled());
		if (result.index() == 0 && !signal_ec)
		{
			bool expected = false;
			if (interrupted_.compare_exchange_strong(expected, true))
			{
				static_cast<TaskImpl&>(*this).stop_requested();
				interrupted_ = true;
				stop_cancellation_timers_impl();
			}
		}
	}

	boost::asio::awaitable<void> cpu_cancelled()
	{
        co_await cpu_cancellation_channel_.async_send(
            boost::asio::as_tuple(boost::asio::use_awaitable));
	}

	[[nodiscard]]
	boost::asio::awaitable<void> processing_task(io_result_type result,
		task_weight_type task_weight)
	{
		if (interrupted_.load(std::memory_order_relaxed))
			co_return;

		co_await boost::asio::this_coro::throw_if_cancelled(false);

		try
		{
			co_await static_cast<TaskImpl&>(*this).cpu_task_impl(std::move(result));
		}
		catch (const std::exception&)
		{
		}

		if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
			co_return;
		if (interrupted_.load(std::memory_order_relaxed))
			co_return;

		using namespace boost::asio::experimental::awaitable_operators;
		boost::asio::co_spawn(io_pool_,
			(notify_processing_task_complete(std::move(task_weight)) || io_cancelled()),
			boost::asio::detached);
	}

	[[nodiscard]]
	boost::asio::awaitable<void> notify_processing_task_complete(
		task_weight_type task_weight)
	{
		assert(io_pool_.get_executor().running_in_this_thread());
		if (!!(co_await boost::asio::this_coro::cancellation_state).cancelled())
			co_return;

		co_await cpu_task_complete_channel_.async_send(boost::system::error_code{},
			std::move(task_weight), boost::asio::use_awaitable);
	}

	[[nodiscard]]
	boost::asio::awaitable<void> io_task()
	{
		assert(io_pool_.get_executor().running_in_this_thread());

		core::scoped_guard guard([this] {
			if (io_signal_set_)
				io_signal_set_->cancel();
		});

		co_await boost::asio::this_coro::throw_if_cancelled(false);

		co_await static_cast<TaskImpl&>(*this).io_task_impl();

		while (total_task_weight_
			&& !(co_await boost::asio::this_coro::cancellation_state).cancelled())
		{
			auto [ec, task_weight] = co_await cpu_task_complete_channel_
				.async_receive(boost::asio::as_tuple(boost::asio::use_awaitable));
			if (ec)
				co_return;

			total_task_weight_ -= task_weight;
		}

		co_await static_cast<TaskImpl&>(*this).on_all_tasks_complete();
	}

	static boost::asio::awaitable<void> cancelled(boost::asio::steady_timer& timer)
	{
		boost::system::error_code ec;
		co_await timer.async_wait(boost::asio::redirect_error(boost::asio::use_awaitable, ec));
	}

private:
	using task_complete_channel_type = boost::asio::experimental::channel<
		void(boost::system::error_code, task_weight_type)>;

private:
	task_weight_type max_task_weight_;
	execution_context_type pool_;
	execution_context_type io_pool_;
	task_complete_channel_type cpu_task_complete_channel_;
	task_weight_type total_task_weight_{};
	std::atomic<bool> interrupted_{};
	core::time_tracker time_tracker_;
	std::optional<boost::asio::signal_set> io_signal_set_;
	boost::asio::steady_timer io_cancel_timer_{ io_pool_,
		std::chrono::steady_clock::time_point::max() };
	boost::asio::experimental::concurrent_channel<void()> cpu_cancellation_channel_{ pool_ };
};

} //namespace bv::thread
