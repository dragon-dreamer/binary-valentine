#include "binary_valentine/progress/terminal_progress_report.h"

#include <iostream>
#include <syncstream>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv::progress
{

void terminal_progress_report::report_progress(
	const std::shared_ptr<const core::subject_entity_interface>& entity,
	progress_state state) noexcept
{
	try
	{
		std::osyncstream stream(std::cout);
		if (entity)
			stream << entity->get_name() << ": ";
		stream << resource_provider_.get_string(progress_state_to_string(state)) << '\n';
	}
	catch (...)
	{
	}
}

} //namespace bv::progress
