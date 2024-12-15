#pragma once

#include <memory>

#include "binary_valentine/core/async_data_generator_interface.h"
#include "binary_valentine/core/combined_data_generator.h"
#include "binary_valentine/core/data_generator_interface.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/rule_detector_container.h"
#include "binary_valentine/core/value_provider.h"
#include "binary_valentine/output/exception_formatter.h"
#include "binary_valentine/output/unformatted_common_output_report.h"

namespace bv::analysis
{

class [[nodiscard]] immutable_context
{
public:
	explicit immutable_context(
		std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader = nullptr);

	[[nodiscard]]
	const bv::core::data_generator_list& get_generators() const noexcept
	{
		return generators_;
	}

	[[nodiscard]]
	const bv::core::async_data_generator_list& get_async_generators() const noexcept
	{
		return async_generators_;
	}

	[[nodiscard]]
	const bv::core::combined_data_generator_list& get_combined_generators() const noexcept
	{
		return combined_generators_;
	}

	[[nodiscard]]
	const bv::core::rule_list& get_rules() const noexcept
	{
		return rules_;
	}

	[[nodiscard]]
	const bv::core::combined_rule_list& get_combined_rules() const noexcept
	{
		return combined_rules_;
	}

	[[nodiscard]]
	const core::rule_detector_interface& get_detector() const noexcept
	{
		return detector_;
	}

	[[nodiscard]]
	const output::exception_formatter& get_exception_formatter() const noexcept
	{
		return exception_formatter_;
	}

	[[nodiscard]]
	const output::unformatted_common_output_report& get_global_report() const noexcept
	{
		return global_reporter_;
	}
	
	[[nodiscard]]
	output::report_uid get_report_uid(std::string_view report_string_uid) const;

	[[nodiscard]]
	const core::embedded_resource_loader_interface& get_embedded_resource_loader() const noexcept
	{
		return *embedded_resource_loader_;
	}

private:
	static core::data_generator_list create_shared_generators(
		std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader);

private:
	output::exception_formatter exception_formatter_;
	output::unformatted_common_output_report global_reporter_;
	core::data_generator_list shared_generators_;
	core::value_provider shared_values_;
	core::data_generator_list generators_;
	core::async_data_generator_list async_generators_;
	core::rule_list rules_;
	core::rule_detector_container detector_;
	core::combined_data_generator_list combined_generators_;
	core::combined_rule_list combined_rules_;
	const core::embedded_resource_loader_interface* embedded_resource_loader_{};
};

} //namespace bv::analysis
