#pragma once

#include <memory>

#include <QObject>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/output/format/output_format_interface.h"
#include "binary_valentine/analysis/analysis_plan_runner.h"

namespace bv
{

namespace output { class common_report_interface; }

class AnalysisRunner : public QObject
{
    Q_OBJECT

public:
    explicit AnalysisRunner(analysis::analysis_plan&& plan,
                            std::shared_ptr<output::common_report_interface> report);

public:
    void requestInterrupt();

public slots:
    void runAnalysisAndWait();

signals:
    void analysisComplete(output::format::analysis_state state);

private:
    analysis::analysis_plan_runner runner_;
    std::shared_ptr<output::common_report_interface> report_;
};

} // namespace bv
