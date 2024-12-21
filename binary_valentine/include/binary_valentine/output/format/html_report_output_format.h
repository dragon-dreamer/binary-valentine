#pragma once

#include <filesystem>
#include <memory>
#include <utility>

#include "binary_valentine/analysis/result_report_file.h"
#include "binary_valentine/output/format/output_format_interface.h"

namespace bv::core { class embedded_resource_loader_interface; }
namespace bv::string { class resource_provider_interface; }

namespace bv::output::format
{

class [[nodiscard]] html_report_output_format final
	: public output_format_interface
{
public:
	explicit html_report_output_format(
		const string::resource_provider_interface& resource_provider,
		std::filesystem::path&& path,
		const analysis::result_report_file::options_map_type& extra_options,
		const core::embedded_resource_loader_interface& resource_loader);

	virtual ~html_report_output_format() override;

	virtual void start(const analysis_state& state,
		const std::optional<extended_analysis_state>& extra_state) override;
	virtual void append(const entity_in_memory_report_interface& report) override;
	virtual void finalize() override;

private:
	struct impl;

	std::filesystem::path path_;
	std::unique_ptr<impl> impl_;
};

} //namespace bv::output::format
