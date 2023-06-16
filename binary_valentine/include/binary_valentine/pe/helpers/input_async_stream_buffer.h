#pragma once

#include <cstddef>
#include <memory>

#include <boost/asio/random_access_file.hpp>

#include "buffers/input_buffer_interface.h"

namespace bv::pe
{

class [[nodiscard]] input_async_stream_buffer final
	: public buffers::input_buffer_interface
{
public:
	explicit input_async_stream_buffer(std::shared_ptr<
		boost::asio::random_access_file> stream);

	[[nodiscard]]
	virtual bool is_stateless() const noexcept override { return false; }

	[[nodiscard]]
	virtual std::size_t size() override;

	virtual std::size_t read(std::size_t pos,
		std::size_t count, std::byte* data) override;

	[[nodiscard]]
	boost::asio::random_access_file& get_file() noexcept
	{
		return *stream_;
	}

private:
	std::shared_ptr<boost::asio::random_access_file> stream_;
	std::size_t size_;
};

} //namespace bv::pe
