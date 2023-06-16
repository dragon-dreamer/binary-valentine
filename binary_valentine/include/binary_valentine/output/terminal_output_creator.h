#pragma once

#include <memory>

#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/terminal_report_output.h"

namespace bv::output
{

struct [[nodiscard]] terminal_output_creator
{
	[[nodiscard]]
	std::shared_ptr<entity_report_interface> operator()(
		const auto& entity, const auto& formatter, const auto& resources) const
	{
		return std::make_shared<terminal_report_output>(entity, resources, formatter);
	}
};

} //namespace bv::output
