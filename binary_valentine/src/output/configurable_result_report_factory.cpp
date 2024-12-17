#include "binary_valentine/output/configurable_result_report_factory.h"

#include <memory>
#include <utility>
#include <variant>

#include "binary_valentine/core/overloaded.h"
#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/output/filtering_report_output.h"
#include "binary_valentine/output/multiple_report_output.h"
#include "binary_valentine/output/in_memory_report_output.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::output
{

configurable_result_report_factory::configurable_result_report_factory(
	const string::resource_provider_interface& resources,
	std::vector<realtime_report_creator_type> realtime_report_creators,
	in_memory_report_creator_type in_memory_report_creator)
	: resources_(resources)
	, realtime_report_creators_(std::move(realtime_report_creators))
	, in_memory_report_creator_(std::move(in_memory_report_creator))
{
}

std::shared_ptr<entity_report_interface>
configurable_result_report_factory::create_in_memory_output(
	const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
	const exception_formatter& formatter,
	const core::rule_class_mask& detected_rule_types)
{
	auto result = in_memory_report_creator_(
		entity, formatter, detected_rule_types, resources_);
	//TODO: change to lock-free list
	std::lock_guard guard(in_memory_report_mutex_);
	in_memory_reports_.emplace_back(result);
	return result;
}

std::shared_ptr<entity_report_interface>
configurable_result_report_factory::create_output(
	const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
	const exception_formatter& formatter,
	const core::rule_class_mask& detected_rule_types)
{
	if (in_memory_report_creator_ && realtime_report_creators_.empty())
		return create_in_memory_output(entity, formatter, detected_rule_types);

	if (!in_memory_report_creator_ && realtime_report_creators_.size() == 1u)
	{
		return (realtime_report_creators_[0])(
			entity, formatter, detected_rule_types, resources_);
	}

	std::vector<std::shared_ptr<entity_report_interface>> targets;
	for (const auto& factory_creator : realtime_report_creators_)
	{
		targets.emplace_back(factory_creator(
			entity, formatter, detected_rule_types, resources_));
	}

	if (in_memory_report_creator_)
	{
		targets.emplace_back(create_in_memory_output(
			entity, formatter, detected_rule_types));
	}

	return std::make_shared<multiple_report_output>(std::move(targets));
}

std::shared_ptr<entity_report_interface>
configurable_result_report_factory::get_entity_report(
	const std::shared_ptr<const bv::core::subject_entity_interface>& entity,
	const bv::core::rule_selector& selector,
	const exception_formatter& formatter,
	const core::rule_class_mask& detected_rule_types)
{
	return std::make_shared<filtering_report_output>(
		create_output(entity, formatter, detected_rule_types),
		formatter, selector);
}

std::shared_ptr<common_report_interface>
configurable_result_report_factory::get_common_report(
	const bv::core::rule_selector& selector,
	const exception_formatter& formatter)
{
	return std::make_shared<filtering_report_output>(
		create_output(nullptr, formatter, {}),
		formatter, selector
	);
}

std::vector<std::shared_ptr<
	const entity_in_memory_report_interface>>
	configurable_result_report_factory::get_in_memory_reports() &&
{
	return std::move(in_memory_reports_);
}

} //namespace bv::output
