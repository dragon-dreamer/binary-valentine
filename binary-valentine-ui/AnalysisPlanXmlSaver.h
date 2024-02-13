#pragma once

#include <QString>

#include "binary_valentine/analysis/analysis_plan.h"

namespace bv
{

class AnalysisPlanXmlSaver final
{
public:
    AnalysisPlanXmlSaver() = delete;

    static void saveToXml(const analysis::analysis_plan& plan, QString path);

    static QString toXml(const analysis::analysis_plan& plan);
};

} // namespace bv
