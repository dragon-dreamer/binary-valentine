#pragma once

#include <atomic>
#include <cassert>
#include <csignal>
#include <cstddef>
#include <exception>
#include <stop_token>
#include <utility>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/channel.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/system/system_error.hpp>

#include "binary_valentine/core/time_tracker.h"
#include "binary_valentine/core/scoped_guard.h"

namespace bv::thread
{

/*
TaskImpl provides:
 * boost::asio::awaitable<void> io_task_impl();
 * boost::asio::awaitable<void> cpu_task_impl(io_result_type result, std::stop_token);
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
		interrupted_ = true;
		stop_source_.request_stop();
		pool_.stop();
		io_pool_.stop();
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
		stop();
		join();
	}

protected:
	[[nodiscard]]
	boost::asio::awaitable<void> post_io_result_to_cpu(io_result_type result)
	{
		assert(io_pool_.get_executor().running_in_this_thread());

		auto weight = static_cast<TaskImpl&>(*this).get_task_weight(result);
		total_task_weight_ += weight;

		boost::asio::co_spawn(pool_, processing_task(std::move(result),
			std::move(weight)), boost::asio::detached);

		while (total_task_weight_ >= max_task_weight_)
		{
			auto task_weight = co_await cpu_task_complete_channel_.async_receive(
				boost::asio::use_awaitable);
			total_task_weight_ -= task_weight;
		}

		while (cpu_task_complete_channel_.try_receive([this](
			const boost::system::error_code&, auto&& task_weight) {
			total_task_weight_ -= task_weight;
		}));
	}

	[[nodiscard]]
	std::stop_token get_stop_token() noexcept
	{
		return stop_source_.get_token();
	}

	[[nodiscard]]
	std::exception_ptr get_io_task_error() const noexcept
	{
		return io_task_error_;
	}

	void start_time_tracker()
	{
		time_tracker_.start();
	}

	void start()
	{
		boost::asio::co_spawn(io_pool_, io_task(),
			[this](std::exception_ptr err) {
				io_task_error_ = std::move(err);
			});
	}

	void enable_signal_cancellation()
	{
		io_signal_set_.emplace(io_pool_, SIGINT, SIGTERM).async_wait(
			[this](const boost::system::error_code& error, int /* signal_number */) {
			if (!error)
			{
				stop();
				static_cast<TaskImpl&>(*this).stop_requested();
			}
		});
	}

private:
	[[nodiscard]]
	boost::asio::awaitable<void> processing_task(io_result_type result,
		task_weight_type task_weight)
	{
		co_await boost::asio::post(co_await boost::asio::this_coro::executor,
			boost::asio::use_awaitable);

		try
		{
			co_await static_cast<TaskImpl&>(*this).cpu_task_impl(std::move(result),
				stop_source_.get_token());
		}
		catch (const std::exception&)
		{
		}

		boost::asio::co_spawn(io_pool_,
			notify_processing_task_complete(std::move(task_weight)),
			boost::asio::detached);
	}

	[[nodiscard]]
	boost::asio::awaitable<void> notify_processing_task_complete(
		task_weight_type task_weight)
	{
		assert(io_pool_.get_executor().running_in_this_thread());
		co_await cpu_task_complete_channel_.async_send(boost::system::error_code{},
			std::move(task_weight), boost::asio::use_awaitable);
	}

	[[nodiscard]]
	boost::asio::awaitable<void> io_task()
	{
		core::scoped_guard guard([this] {
			if (io_signal_set_)
				io_signal_set_->cancel();
		});

		assert(io_pool_.get_executor().running_in_this_thread());

		co_await static_cast<TaskImpl&>(*this).io_task_impl();

		while (total_task_weight_)
		{
			total_task_weight_ -= co_await cpu_task_complete_channel_
				.async_receive(boost::asio::use_awaitable);
		}

		co_await static_cast<TaskImpl&>(*this).on_all_tasks_complete();
	}

private:
	using task_complete_channel_type = boost::asio::experimental::channel<
		void(boost::system::error_code, task_weight_type)>;

private:
	task_weight_type max_task_weight_;
	execution_context_type pool_;
	execution_context_type io_pool_;
	task_complete_channel_type cpu_task_complete_channel_;
	std::stop_source stop_source_;
	task_weight_type total_task_weight_{};
	std::exception_ptr io_task_error_{};
	std::atomic<bool> interrupted_{};
	core::time_tracker time_tracker_;
	std::optional<boost::asio::signal_set> io_signal_set_;
};

} //namespace bv::thread
