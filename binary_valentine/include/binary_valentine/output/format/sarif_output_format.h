#pragma once

#include <filesystem>
#include <memory>
#include <utility>

#include "binary_valentine/output/format/output_format_interface.h"

namespace bv::string { class resource_provider_interface; }

namespace bv::output::format
{

class [[nodiscard]] sarif_output_format final
	: public output_format_interface
{
public:
	explicit sarif_output_format(
		const string::resource_provider_interface& resource_provider,
		std::filesystem::path&& path) noexcept
		: resource_provider_(resource_provider)
		, path_(std::move(path))
	{
	}

	virtual void start(const analysis_state& state) override;
	virtual void append(const entity_in_memory_report_interface& report) override;
	virtual void finalize() override;

private:
	struct impl;

	const string::resource_provider_interface& resource_provider_;
	std::filesystem::path path_;
	std::shared_ptr<impl> impl_;
};

} //namespace bv::output::format
