#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

#include "binary_valentine/analysis/analysis_context.h"
#include "binary_valentine/core/async_value_provider.h"
#include "binary_valentine/core/rule_class_mask.h"
#include "binary_valentine/core/rule_selector.h"
#include "binary_valentine/file/async_target_enumerator.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/progress/progress_report_interface.h"
#include "binary_valentine/thread/multi_executor_concurrent_io_processing_service.h"

namespace bv::core { class subject_entity_interface; }

namespace bv::file
{
struct target_entry;
} //namespace bv::file

namespace bv::analysis
{

class analysis_plan;
class immutable_context;

namespace impl
{
struct loaded_target
{
	std::reference_wrapper<const core::rule_selector> selector;
	core::rule_class_mask rule_types;
	std::shared_ptr<core::async_value_provider> value_provider;
	std::shared_ptr<output::entity_report_interface> report;
	std::shared_ptr<const core::subject_entity_interface> entity;
	std::uint64_t file_size{};
};
} //namespace impl

class concurrent_analysis_executor
	: public thread::multi_executor_concurrent_io_processing_service<
		concurrent_analysis_executor, impl::loaded_target, std::uint64_t>
{
public:
	explicit concurrent_analysis_executor(
		const analysis_plan& plan,
		output::result_report_factory_interface& report_factory,
		const std::shared_ptr<progress::progress_report_interface>& progress_report,
		const immutable_context& global_context);

protected:
	friend thread::multi_executor_concurrent_io_processing_service<
		concurrent_analysis_executor, impl::loaded_target, std::uint64_t>;

	[[nodiscard]]
	boost::asio::awaitable<void> io_task_impl();

	[[nodiscard]]
	boost::asio::awaitable<void> on_all_tasks_complete();
	
	[[nodiscard]]
	boost::asio::awaitable<void> cpu_task_impl(
		io_result_type result, std::stop_token stop_token);

	[[nodiscard]]
	std::uint64_t get_task_weight(const io_result_type& result) const noexcept;

	void stop_requested();

private:
	[[nodiscard]]
	boost::asio::awaitable<void> load_target(file::target_entry&& result);

	void report_progress(const std::shared_ptr<const core::subject_entity_interface>& entity,
		progress::progress_state state) noexcept;

private:
	const analysis_plan& plan_;
	analysis_context context_;
	std::shared_ptr<output::common_report_interface> common_report_;
	std::shared_ptr<progress::progress_report_interface> progress_report_;
	file::async_target_enumerator::target_filtered_callback_type target_filtered_callback_;
};

} //namespace bv::analysis
