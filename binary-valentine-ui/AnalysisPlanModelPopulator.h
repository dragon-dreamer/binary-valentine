#pragma once

#include "binary_valentine/analysis/analysis_plan.h"

namespace bv
{

class ProjectModel;

class AnalysisPlanModelPopulator final
{
public:
    AnalysisPlanModelPopulator() = delete;

    static void populateModel(const analysis::analysis_plan& plan, ProjectModel& model);
};

} // namespace bv
