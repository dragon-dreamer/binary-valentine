#pragma once

#include <span>
#include <unordered_set>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/async_value_provider.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/value.h"

namespace bv::output { class common_report_interface; }

namespace bv::core
{

class combined_dependency_helper final
{
public:
	combined_dependency_helper() = delete;

	static void get_individual_dependencies(
		const combined_data_generator_list& generators,
		std::span<const optional_dependency> combined_deps,
		std::unordered_set<value_tag>& individual_deps,
		output::common_report_interface& report);

	static void get_individual_dependencies(
		const combined_data_generator_list& generators,
		const enabled_combined_rule_list& rules,
		std::unordered_set<value_tag>& individual_deps,
		output::common_report_interface& report);

	[[nodiscard]]
	static boost::asio::awaitable<void> prepare_individual_dependencies(
		const std::unordered_set<value_tag>& individual_deps,
		async_value_provider& values,
		bool remove_other_dependencies = true);
};

} //namespace bv::core
