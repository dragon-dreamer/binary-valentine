#pragma once

#include <cassert>
#include <cstddef>
#include <optional>
#include <stop_token>
#include <utility>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/concurrent_channel.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/system/system_error.hpp>

namespace bv::thread
{

/*
TaskImpl provides:
 * boost::asio::awaitable<void> io_task_impl();
 * boost::asio::awaitable<void> cpu_task_impl(io_result_type result, std::stop_token);
*/
template<typename TaskImpl, typename IoResultType>
class [[nodiscard]] concurrent_io_processing_service
{
public:
	using io_result_type = IoResultType;
	using execution_context_type = boost::asio::thread_pool;

public:
	explicit concurrent_io_processing_service(std::size_t cpu_thread_count,
		std::size_t max_buffered_files)
		: cpu_thread_count_(cpu_thread_count)
		, pool_(cpu_thread_count + 1u)
		, channel_(pool_, max_buffered_files)
	{
	}

	void join()
	{
		pool_.join();
	}

	void stop()
	{
		stop_source_.request_stop();
		pool_.stop();
	}

	~concurrent_io_processing_service()
	{
		stop();
		join();
	}

protected:
	[[nodiscard]]
	boost::asio::awaitable<void> post_io_result_to_cpu(io_result_type result)
	{
		co_await channel_.async_send(boost::system::error_code{},
			std::move(result), boost::asio::use_awaitable);
	}

	[[nodiscard]]
	auto& get_executor() noexcept
	{
		return pool_;
	}

	[[nodiscard]]
	std::stop_token get_stop_token() noexcept
	{
		return stop_source_.get_token();
	}

	void start()
	{
		boost::asio::co_spawn(pool_, io_task(),
			boost::asio::detached);

		for (std::size_t i = 0; i != cpu_thread_count_; ++i)
		{
			boost::asio::co_spawn(pool_, cpu_task(),
				boost::asio::detached);
		}
	}

private:
	[[nodiscard]]
	boost::asio::awaitable<void> cpu_task()
	{
		while (!stop_source_.stop_requested())
		{
			auto data = co_await channel_.async_receive(boost::asio::use_awaitable);
			if (!data) //No more data
				co_return;
			co_await static_cast<TaskImpl&>(*this).cpu_task_impl(std::move(data.value()),
				stop_source_.get_token());
		}
	}

	[[nodiscard]]
	boost::asio::awaitable<void> io_task()
	{
		co_await static_cast<TaskImpl&>(*this).io_task_impl();

		for (std::size_t i = 0; i != cpu_thread_count_; ++i)
		{
			co_await channel_.async_send(boost::system::error_code{},
				std::nullopt, boost::asio::use_awaitable);
		}
	}

private:
	using channel_type = boost::asio::experimental::concurrent_channel<
		void(boost::system::error_code, std::optional<io_result_type>)>;

private:
	std::size_t cpu_thread_count_;
	execution_context_type pool_;
	channel_type channel_;
	std::stop_source stop_source_;
};

} //namespace bv::thread
