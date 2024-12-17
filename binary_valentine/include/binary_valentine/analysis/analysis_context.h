#pragma once

#include <memory>
#include <mutex>
#include <stop_token>
#include <unordered_set>
#include <utility>
#include <vector>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/rule_class_mask.h"
#include "binary_valentine/core/rule_list.h"
#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/core/value.h"

namespace bv::output
{
class common_report_interface;
class entity_report_interface;
class result_report_factory_interface;
} //namespace bv::output

namespace bv::core
{
class async_value_provider;
class async_value_provider_interface;
} //namespace bv::core

namespace bv::analysis
{

class analysis_plan;
class immutable_context;

class analysis_context final
{
public:
	using entity_report_list_type = std::vector<
		std::shared_ptr<const output::entity_report_interface>>;

public:
	explicit analysis_context(
		const analysis_plan& plan,
		output::result_report_factory_interface& report_factory,
		const immutable_context& global_context);

	[[nodiscard]]
	const analysis_plan& get_plan() const noexcept
	{
		return plan_;
	}

	[[nodiscard]]
	output::result_report_factory_interface& get_report_factory() noexcept
	{
		return report_factory_;
	}

	[[nodiscard]]
	const immutable_context& get_global_context() const noexcept
	{
		return global_context_;
	}

	[[nodiscard]]
	bool do_combined_analysis() const noexcept;

	[[nodiscard]]
	boost::asio::awaitable<void> store_values_for_combined_analysis(
		std::shared_ptr<core::async_value_provider>&& values,
		const std::shared_ptr<output::entity_report_interface>& entity_report,
		const core::rule_class_mask& rule_types,
		std::reference_wrapper<const core::rule_selector> selector);

	void run_combined_analysis(std::stop_token stop_token);

private:
	struct combined_context
	{
		using value_provider_list_type = std::vector<
			std::shared_ptr<core::value_provider_interface_base>>;
		std::vector<value_provider_list_type> value_providers_per_class;
	};

	void run_combined_analysis(std::size_t rule_class_index,
		const std::stop_token& stop_token);

private:
	const analysis_plan& plan_;
	output::result_report_factory_interface& report_factory_;
	const immutable_context& global_context_;
	std::mutex combined_context_mutex_;
	combined_context combined_context_;
	std::vector<core::enabled_combined_rule_list> combined_rules_per_class_;
	std::shared_ptr<output::common_report_interface> common_report_;
	std::unordered_set<core::value_tag> required_individual_dependencies_;
	std::mutex entity_reports_mutex_;
	bool has_combined_rules_ = false;
};

} //namespace bv::analysis
