#include "binary_valentine/file/async_file.h"

#include <boost/predef/os/windows.h>

#if BOOST_OS_WINDOWS
#	include <Windows.h>
#endif //BOOST_OS_WINDOWS

#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/stream_file.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/system/error_code.hpp>

namespace bv::file
{

namespace
{
template<typename File>
void open_read_impl(
    const std::filesystem::path& path,
    [[maybe_unused]] bool random_access,
    File& file)
{
#if BOOST_OS_WINDOWS
    const DWORD access = GENERIC_READ;
    const DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;
    const DWORD disposition = OPEN_EXISTING;
    DWORD flags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED;

    if (random_access)
        flags |= FILE_FLAG_RANDOM_ACCESS;

    HANDLE handle = ::CreateFileW(path.native().c_str(),
        access, share, 0, disposition, flags, 0);
    if (handle == INVALID_HANDLE_VALUE)
    {
        DWORD last_error = ::GetLastError();
        boost::system::error_code ec;
        ec.assign(last_error, boost::asio::error::get_system_category());
        BOOST_ASIO_ERROR_LOCATION(ec);
        boost::asio::detail::throw_error(ec, "async_file::open_read");
    }

    file.assign(handle);
#else //BOOST_OS_WINDOWS
    file.open(path.native().c_str(), boost::asio::file_base::read_only);
#endif //BOOST_OS_WINDOWS
}
} //namespace

void async_file::open_read(
	const std::filesystem::path& path,
    boost::asio::random_access_file& file)
{
    open_read_impl(path, true, file);
}

boost::asio::awaitable<async_file_read_result> async_file::read_async(
    const std::filesystem::path& path,
    std::vector<std::byte>& result,
    std::size_t max_size)
{
    boost::asio::stream_file file(co_await boost::asio::this_coro::executor);

    try
    {
        open_read_impl(path, false, file);
    }
    catch (const boost::system::system_error& e)
    {
        if (e.code() == boost::system::errc::no_such_file_or_directory)
            co_return async_file_read_result::does_not_exist;

        throw;
    }

    const auto file_size = file.size();
    if (file.size() > max_size)
        co_return async_file_read_result::too_big;

    result.resize(static_cast<std::size_t>(file_size));

    auto read = co_await boost::asio::async_read(file,
        boost::asio::mutable_buffer(result.data(), result.size()),
        boost::asio::use_awaitable);

    if (read != result.size())
        throw std::runtime_error("Unable to read the full file data");

    co_return async_file_read_result::ok;
}

} //namespace bv::file
