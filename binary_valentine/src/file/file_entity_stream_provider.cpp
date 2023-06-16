#include "binary_valentine/file/file_entity_stream_provider.h"

#include <fstream>
#include <utility>

#include "binary_valentine/file/async_file.h"

namespace bv::file
{

file_entity_stream_provider::file_entity_stream_provider(
	const std::filesystem::path& path)
	: path_(path)
{
}

std::shared_ptr<boost::asio::random_access_file> file_entity_stream_provider::get(
	boost::asio::any_io_executor executor)
{
	if (!stream_)
	{
		auto stream = std::make_shared<boost::asio::random_access_file>(std::move(executor));
		file::async_file::open_read(path_, *stream);
		stream_ = std::move(stream);
	}

	return stream_;
}

} //namespace bv::file
