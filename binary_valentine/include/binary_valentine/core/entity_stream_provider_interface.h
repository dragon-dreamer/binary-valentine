#pragma once

#include <memory>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/random_access_file.hpp>

namespace bv::core
{

class [[nodiscard]] entity_stream_provider_interface
{
public:
	virtual ~entity_stream_provider_interface() = default;

	[[nodiscard]]
	virtual std::shared_ptr<boost::asio::random_access_file> get(
		boost::asio::any_io_executor executor) = 0;
};

} //namespace bv::core
