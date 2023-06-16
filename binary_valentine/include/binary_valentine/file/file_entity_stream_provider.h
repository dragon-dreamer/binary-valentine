#pragma once

#include <filesystem>
#include <memory>

#include "binary_valentine/core/entity_stream_provider_interface.h"

namespace bv::file
{

class [[nodiscard]] file_entity_stream_provider final
	: public core::entity_stream_provider_interface
{
public:
	explicit file_entity_stream_provider(const std::filesystem::path& path);

	[[nodiscard]]
	virtual std::shared_ptr<boost::asio::random_access_file> get(
		boost::asio::any_io_executor executor) override;

private:
	std::filesystem::path path_;
	std::shared_ptr<boost::asio::random_access_file> stream_;
};

} //namespace bv::file
