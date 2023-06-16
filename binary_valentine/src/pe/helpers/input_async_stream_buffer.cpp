#include "binary_valentine/pe/helpers/input_async_stream_buffer.h"

#include <istream>
#include <utility>

#include <boost/asio/buffer.hpp>
#include <boost/asio/read_at.hpp>

#include "utilities/generic_error.h"
#include "utilities/math.h"
#include "utilities/scoped_guard.h"

namespace bv::pe
{
input_async_stream_buffer::input_async_stream_buffer(
    std::shared_ptr<boost::asio::random_access_file> stream)
    : stream_(std::move(stream))
    , size_{}
{
    size_ = static_cast<std::size_t>(stream_->size());
}

std::size_t input_async_stream_buffer::size()
{
    return size_;
}

std::size_t input_async_stream_buffer::read(std::size_t pos,
    std::size_t count, std::byte* data)
{
    static_assert(sizeof(std::byte) == sizeof(char));

    if (!count)
        return 0u;

    if (!utilities::math::is_sum_safe(pos, count) || pos + count > size_)
        throw std::system_error(utilities::generic_errc::buffer_overrun);

    auto read = boost::asio::read_at(*stream_,
        pos, boost::asio::mutable_buffer(data, count));
    if (read != count)
        throw std::system_error(utilities::generic_errc::buffer_overrun);
    return count;
}
} //namespace bv::pe
