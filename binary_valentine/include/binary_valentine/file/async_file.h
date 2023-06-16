#pragma once

#include <cstddef>
#include <filesystem>
#include <vector>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/random_access_file.hpp>

namespace bv::file
{

enum class async_file_read_result
{
	ok,
	too_big,
	does_not_exist
};

class async_file final
{
public:
	async_file() = delete;
	
	static void open_read(
		const std::filesystem::path& path,
		boost::asio::random_access_file& file);

	[[nodiscard]]
	static boost::asio::awaitable<async_file_read_result> read_async(
		const std::filesystem::path& path,
		std::vector<std::byte>& result,
		std::size_t max_size);
};

} //namespace bv::file
