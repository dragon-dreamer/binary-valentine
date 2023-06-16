#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "binary_valentine/output/format/output_format_interface.h"

namespace bv::output { class entity_in_memory_report_interface; }

namespace bv::output::format
{

class [[nodiscard]] output_format_executor final
{
public:
	explicit output_format_executor(std::vector<std::shared_ptr<
		const entity_in_memory_report_interface>> reports);

	void save_to(const std::shared_ptr<output_format_interface>& output,
		const analysis_state& state) const;

private:
	std::vector<std::shared_ptr<
		const entity_in_memory_report_interface>> entity_reports_;
	std::unique_ptr<const entity_in_memory_report_interface> common_report_;
};

} //namespace bv::output::format
