#pragma once

#include <memory>

#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/in_memory_report_output.h"

namespace bv::output
{

struct [[nodiscard]] in_memory_output_creator
{
	[[nodiscard]]
	std::shared_ptr<in_memory_report_output> operator()(
		const auto& entity, const auto& formatter, const auto& resources) const
	{
		return std::make_shared<in_memory_report_output>(entity, resources, formatter);
	}
};

} //namespace bv::output
