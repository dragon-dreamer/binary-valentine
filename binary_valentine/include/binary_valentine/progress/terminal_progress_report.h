#pragma once

#include "binary_valentine/progress/progress_report_interface.h"

namespace bv::string { class resource_provider_interface; }

namespace bv::progress
{

class [[nodiscard]] terminal_progress_report final
	: public progress_report_interface
{
public:
	explicit terminal_progress_report(
		const string::resource_provider_interface& resource_provider) noexcept
		: resource_provider_(resource_provider)
	{
	}

	virtual void report_progress(
		const std::shared_ptr<const core::subject_entity_interface>& entity,
		progress_state state) noexcept override;

private:
	const string::resource_provider_interface& resource_provider_;
};

} //namespace bv::progress
