#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/url.hpp>

namespace bv::https
{

class https_request_helper final
{
public:
	struct options
	{
		std::size_t resolve_timeout = 5u;
		std::size_t connect_timeout = 5u;
		std::size_t send_timeout = 10u;
		std::size_t read_timeout = 10u;
	};

public:
	static boost::asio::awaitable<std::string> get(
		const boost::url_view& url, const options& opts = {});
};

} //namespace bv::https
