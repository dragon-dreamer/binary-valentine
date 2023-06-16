#include "binary_valentine/output/format/output_format_executor.h"

#include <algorithm>
#include <ranges>
#include <utility>

#include "binary_valentine/output/entity_in_memory_report_interface.h"
#include "binary_valentine/output/format/output_format_interface.h"

namespace bv::output::format
{

namespace
{

class common_in_memory_report final : public entity_in_memory_report_interface
{
public:
	[[nodiscard]]
	virtual const saved_rule_reports_type& get_rule_reports() const noexcept override
	{
		return empty_;
	}

	[[nodiscard]]
	virtual const saved_common_reports_type& get_common_reports() const noexcept
	{
		return common_;
	}

	[[nodiscard]]
	virtual const core::subject_entity_interface* get_entity() const noexcept override
	{
		return nullptr;
	}

	[[nodiscard]]
	saved_common_reports_type& get_common_reports() noexcept
	{
		return common_;
	}

private:
	saved_rule_reports_type empty_;
	saved_common_reports_type common_;
};
} //namespace

output_format_executor::output_format_executor(std::vector<std::shared_ptr<
	const entity_in_memory_report_interface>> reports)
	: entity_reports_(std::move(reports))
{
	if (entity_reports_.empty())
		return;

	auto first_with_no_entity = std::partition(
		entity_reports_.begin(), entity_reports_.end(),
		[](const auto& report) {
			return report->get_entity() != nullptr;
		});

	auto merged_common_reports = std::make_unique<common_in_memory_report>();
	for (auto it = first_with_no_entity; it != entity_reports_.end(); ++it)
	{
		merged_common_reports->get_common_reports().insert(
			merged_common_reports->get_common_reports().end(),
			std::move_iterator(it->get()->get_common_reports().begin()),
			std::move_iterator(it->get()->get_common_reports().end())
		);
	}

	entity_reports_.erase(first_with_no_entity, entity_reports_.end());
	common_report_ = std::move(merged_common_reports);
}

void output_format_executor::save_to(
	const std::shared_ptr<output_format_interface>& output,
	const analysis_state& state) const
{
	output->start(state);
	if (common_report_)
	{
		if (!common_report_->get_common_reports().empty()
			|| !common_report_->get_rule_reports().empty())
		{
			output->append(*common_report_);
		}
	}

	for (const auto& report : entity_reports_)
		output->append(*report);

	output->finalize();
}

} //namespace bv::output::format
