#pragma once

#include <filesystem>
#include <string>
#include <variant>

#include "binary_valentine/analysis/analysis_plan_provider_interface.h"

namespace bv::analysis
{

class immutable_context;

struct analysis_plan_options
{
	bool allow_empty_targets = false;
	bool allow_empty_reports = false;
};

class [[nodiscard]] xml_analysis_plan_provider final
	: public analysis_plan_provider_interface
{
public:
	explicit xml_analysis_plan_provider(std::filesystem::path&& path,
		const immutable_context& context,
		const analysis_plan_options& options = {});

	explicit xml_analysis_plan_provider(std::string&& xml,
		const immutable_context& context,
		const analysis_plan_options& options = {});

	[[nodiscard]]
	virtual analysis_plan get() const override;

private:
	std::variant<std::filesystem::path, std::string> xml_;
	const immutable_context& context_;
	analysis_plan_options options_;
};

} //namespace bv::analysis
