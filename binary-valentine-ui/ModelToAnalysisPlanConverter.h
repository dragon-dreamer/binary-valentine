#pragma once

#include "binary_valentine/analysis/analysis_plan.h"

namespace bv
{

struct ProjectTreeNodes;

struct ModelToAnalysisPlanConverterOpts final
{
    bool skipTerminalOutput = false;
};

class ModelToAnalysisPlanConverter final
{
public:
    ModelToAnalysisPlanConverter() = delete;

    [[nodiscard]]
    static analysis::analysis_plan toPlan(const ProjectTreeNodes& nodes,
                                          const ModelToAnalysisPlanConverterOpts& opts = {});
};

} // namespace bv
