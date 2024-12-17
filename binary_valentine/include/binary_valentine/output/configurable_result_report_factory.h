#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <variant>
#include <vector>

#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/output/in_memory_report_creator_type.h"
#include "binary_valentine/output/format/output_format_interface.h"
#include "binary_valentine/output/realtime_report_creator_type.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::string { class resource_provider_interface; }

namespace bv::output
{

class [[nodiscard]] configurable_result_report_factory final
	: public result_report_factory_interface
{
public:
	explicit configurable_result_report_factory(
		const string::resource_provider_interface& resources,
		std::vector<realtime_report_creator_type> realtime_report_creators,
		in_memory_report_creator_type in_memory_report_creator);

	[[nodiscard]]
	virtual std::shared_ptr<entity_report_interface> get_entity_report(
		const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
		const bv::core::rule_selector& selector,
		const exception_formatter& formatter,
		const bv::core::rule_class_mask& detected_rule_types) override;

	[[nodiscard]]
	virtual std::shared_ptr<common_report_interface> get_common_report(
		const bv::core::rule_selector& selector,
		const exception_formatter& formatter) override;

public:
	[[nodiscard]]
	std::vector<std::shared_ptr<
		const entity_in_memory_report_interface>> get_in_memory_reports() &&;

private:
	std::shared_ptr<entity_report_interface> create_output(
		const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
		const exception_formatter& formatter,
		const bv::core::rule_class_mask& detected_rule_types);
	std::shared_ptr<entity_report_interface> create_in_memory_output(
		const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
		const exception_formatter& formatter,
		const bv::core::rule_class_mask& detected_rule_types);

private:
	const string::resource_provider_interface& resources_;
	std::vector<realtime_report_creator_type> realtime_report_creators_;
	in_memory_report_creator_type in_memory_report_creator_;
	std::mutex in_memory_report_mutex_;
	std::vector<std::shared_ptr<
		const entity_in_memory_report_interface>> in_memory_reports_;
};

} //namespace bv::output
