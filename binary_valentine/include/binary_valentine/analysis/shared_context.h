#pragma once

#include <memory>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/async_data_generator_interface.h"
#include "binary_valentine/core/async_value_provider.h"
#include "binary_valentine/core/data_generator_interface.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/shared_value_provider.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/output/unformatted_common_output_report.h"

namespace bv::analysis
{

// Shared context is mutable and may be used by multiple analysis processes
// (`analysis_plan_runner` instances), but not in parallel.
// It loads shared data and settings and caches the results so they can
// be reused in the next analysis.
class [[nodiscard]] shared_context
{
public:
	explicit shared_context(
		std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader = nullptr);

	core::shared_value_provider& get_shared_values() noexcept
	{
		return shared_values_;
	}

	[[nodiscard]]
	const output::unformatted_common_output_report& get_global_report() const noexcept
	{
		return global_reporter_;
	} 
	
	[[nodiscard]]
	const core::embedded_resource_loader_interface& get_embedded_resource_loader() const noexcept
	{
		return *embedded_resource_loader_;
	}

private:
	static core::async_data_generator_list create_shared_generators();
	static core::data_generator_list create_sync_shared_generators(
		std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader);
	void generate_sync_data();

private:
	output::unformatted_common_output_report global_reporter_;
	core::data_generator_list sync_data_generators_;
	core::value_provider sync_values_;
	core::async_data_generator_list shared_generators_;
	core::shared_value_provider shared_values_;
	const core::embedded_resource_loader_interface* embedded_resource_loader_{};
};

} //namespace bv::analysis
