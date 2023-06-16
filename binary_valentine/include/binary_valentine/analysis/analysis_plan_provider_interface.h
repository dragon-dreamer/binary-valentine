#pragma once

namespace bv::analysis
{

class analysis_plan;

class [[nodiscard]] analysis_plan_provider_interface
{
public:
	virtual ~analysis_plan_provider_interface() = default;

	[[nodiscard]]
	virtual analysis_plan get() const = 0;
};

} //namespace bv::analysis
