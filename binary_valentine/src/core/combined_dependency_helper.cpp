#include "binary_valentine/core/combined_dependency_helper.h"

#include <queue>

#include "binary_valentine/core/combined_data_generator_interface.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{

void combined_dependency_helper::get_individual_dependencies(
	const combined_data_generator_list& generators,
	std::span<const optional_dependency> combined_deps,
	std::unordered_set<value_tag>& individual_deps,
	output::common_report_interface& report)
{
	std::queue<value_tag> combined_to_visit;
	for (auto combined : combined_deps)
		combined_to_visit.push(combined.tag);

	std::unordered_set<value_tag> combined_visited;
	combined_visited.reserve(combined_deps.size());
	for (auto dep : combined_deps)
		combined_visited.emplace(dep.tag);

	while (!combined_to_visit.empty())
	{
		auto tag = combined_to_visit.front();
		combined_to_visit.pop();
		auto gen_ptr = generators.get(tag);
		if (!gen_ptr)
		{
			report.log(output::report_level::critical,
				output::reports::no_generator_for_type,
				output::named_arg(output::arg::tag_name, tag.name()));
			continue;
		}

		auto deps = gen_ptr->get_individual_dependencies();
		individual_deps.insert(deps.begin(), deps.end());

		auto combined_deps = gen_ptr->get_combined_dependencies();
		for (auto dep : combined_deps)
		{
			if (combined_visited.insert(dep).second)
				combined_to_visit.push(dep);
		}
	}
}

void combined_dependency_helper::get_individual_dependencies(
	const combined_data_generator_list& generators,
	const enabled_combined_rule_list& rules,
	std::unordered_set<value_tag>& individual_deps,
	output::common_report_interface& report)
{
	for (const auto& rule : rules.get_rules())
	{
		get_individual_dependencies(generators,
			rule.get().get_combined_dependencies(), individual_deps, report);
		for (auto dep : rule.get().get_individual_dependencies())
			individual_deps.emplace(dep.tag);
	}
}

boost::asio::awaitable<void> combined_dependency_helper::prepare_individual_dependencies(
	const std::unordered_set<value_tag>& individual_deps,
	async_value_provider& values,
	bool remove_other_dependencies)
{
	for (auto dep : individual_deps)
	{
		[[maybe_unused]] const auto* result = co_await values.get_async(dep);
	}

	if (remove_other_dependencies)
	{
		values.remove_if([&individual_deps](value_tag tag)
		{
			return !individual_deps.contains(tag);
		});
	}
}

} //namespace bv::core
