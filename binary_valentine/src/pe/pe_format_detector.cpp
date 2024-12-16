#include "binary_valentine/pe/pe_format_detector.h"

#include <exception>
#include <memory>
#include <utility>

#include <boost/asio/this_coro.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/use_awaitable.hpp>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/core/user_error.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_cache.h"
#include "binary_valentine/pe/helpers/input_async_stream_buffer.h"
#include "binary_valentine/string/case_conv.h"

#include "buffers/input_container_buffer.h"

#include "pe_bliss2/image/format_detector.h"

namespace bv::pe
{

boost::asio::awaitable<bool> pe_format_detector::detect(
    const core::subject_entity_interface& entity,
    core::entity_stream_provider_interface& stream_provider,
    core::value_cache& values,
    std::vector<core::rule_class_type>& rules) const
{
    if (!entity.is_regular_file())
        co_return false;

    const auto& path = entity.get_path();
    if (!path.has_extension())
        co_return false;
    
    {
        auto ext_string = path.extension().string();
        string::to_lower_ascii_inplace(ext_string);
        if (!executable_extensions_.contains(ext_string))
            co_return false;
    }

    auto file = stream_provider.get(co_await boost::asio::this_coro::executor);
    std::shared_ptr<buffers::input_buffer_interface> buf
        = std::make_shared<bv::pe::input_async_stream_buffer>(file);
    const auto detected_format = pe_bliss::image::format_detector::detect_format(*buf);
    if (detected_format == pe_bliss::image::detected_format::none)
        co_return false;

    auto file_size = buf->physical_size();
    if (file_size <= max_in_memory_size)
    {
        auto container_buf = std::make_shared<buffers::input_container_buffer>();
        auto& container = container_buf->get_container();
        container.resize(file_size);

        auto read_bytes = co_await file->async_read_some_at(0u,
            boost::asio::mutable_buffer(container.data(), container.size()),
            boost::asio::use_awaitable);

        if (read_bytes != container.size())
            throw core::user_error(core::user_errc::unable_to_read_file);

        buf = std::move(container_buf);
    }

    values.set(core::make_value<
        std::shared_ptr<buffers::input_buffer_interface>>(std::move(buf)));

    rules.emplace_back(static_cast<core::rule_class_type>(rule_class_type::pe));
    rules.emplace_back(static_cast<core::rule_class_type>(rule_class_type::executable));
    if (detected_format == pe_bliss::image::detected_format::pe32)
        rules.emplace_back(static_cast<core::rule_class_type>(rule_class_type::pe32));
    else if (detected_format == pe_bliss::image::detected_format::pe64)
        rules.emplace_back(static_cast<core::rule_class_type>(rule_class_type::pe64));
    co_return true;
}

} //namespace bv::pe
