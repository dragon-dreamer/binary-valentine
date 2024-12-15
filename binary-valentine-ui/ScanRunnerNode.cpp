#include "ScanRunnerNode.h"

#include <vector>

#include "AnalysisRunner.h"
#include "AnalyzedFileTracker.h"
#include "ImmutableContext.h"
#include "InMemoryReportOutput.h"
#include "ModelToAnalysisPlanConverter.h"
#include "ProgressReport.h"

#include "binary_valentine/core/rule_class.h"
#include "binary_valentine/output/format/output_format_interface.h"
#include "binary_valentine/output/internal_report_messages.h"

namespace bv
{

ScanRunnerNode::ScanRunnerNode(ProjectTreeNode* parent, const ProjectTreeNodes& nodes)
    : ProjectTreeNode(parent, ProjectNodeTypeName::ScanRunner)
    , nodes_(nodes)
    , progressReport_(std::make_shared<ProgressReport>())
    , fileTracker_(std::make_shared<AnalyzedFileTracker>())
{
}

void ScanRunnerNode::startAnalysis()
{
    if (analysisInProgress_)
        return;

    analysisInProgress_ = true;
    analysisStopRequested_ = false;
    selectedFileIndex_ = 0;

    runAnalysis();
    emit analysisInProgressChanged();
}

void ScanRunnerNode::requestAnalysisStop()
{
    if (!analysisInProgress_ || analysisStopRequested_)
        return;

    analysisStopRequested_ = true;
    auto runner = analysisRunner_;
    if (runner)
        runner->requestInterrupt();
}

QVariantList ScanRunnerNode::getAllFiles()
{
    return fileTracker_->getAllFiles(reportCategories_, reportLevels_);
}

AnalysisProgress* ScanRunnerNode::getAnalysisProgress() const
{
    auto progress = std::make_unique<AnalysisProgress>(
        progressReport_->getTotalRead(),
        progressReport_->getTotalAnalyzed(),
        QString::fromStdString(progressReport_->getCurrentAnalyzedTargetPath()));
    progress->setNewFiles(fileTracker_->getNewlyAddedFiles());
    return progress.release();
}

QVariantList ScanRunnerNode::getFileReport()
{
    return fileTracker_->getReports(selectedFileIndex_, reportCategories_, reportLevels_);
}

void ScanRunnerNode::runAnalysis()
{
    fileTracker_ = std::make_shared<AnalyzedFileTracker>();

    auto commonReport = std::make_shared<InMemoryReportOutput>(
        nullptr,
        ImmutableContext::getLocalizedResources(),
        std::vector<bv::core::rule_class_type>{},
        ImmutableContext::getImmutableContext().get_exception_formatter());
    fileTracker_->registerFile(commonReport);

    auto progressReport = std::make_shared<ProgressReport>();
    progressReport_ = progressReport;

    analysis::analysis_plan plan;
    try
    {
        plan = ModelToAnalysisPlanConverter::toPlan(nodes_, { .skipTerminalOutput = true });
    }
    catch (const std::exception&)
    {
        commonReport->log_noexcept(
            output::report_level::critical,
            output::reports::analysis_exception,
            output::current_exception_arg());
        analysisInProgress_ = false;
        emit analysisInProgressChanged();
        return;
    }

    plan.set_progress_report(std::move(progressReport));

    plan.set_custom_in_memory_report_creator(
        [fileTracker = fileTracker_]
        (const auto& entity, const auto& formatter,
                                     const auto& detected_rule_types, const auto& resources) {
            auto report = std::make_shared<InMemoryReportOutput>(
                entity, resources, detected_rule_types, formatter);
            fileTracker->registerFile(report);
            return report;
        });

    analysisRunner_ = std::make_shared<AnalysisRunner>(std::move(plan), std::move(commonReport));
    analysisRunner_->moveToThread(&analysisThread_);
    connect(this, &ScanRunnerNode::kickOffAnalysis,
            analysisRunner_.get(), &AnalysisRunner::runAnalysisAndWait);
    connect(analysisRunner_.get(), &AnalysisRunner::analysisComplete,
            this, &ScanRunnerNode::analysisComplete);

    analysisThread_.start();

    emit kickOffAnalysis();
}

void ScanRunnerNode::analysisComplete(output::format::analysis_state /* state */)
{
    analysisThread_.quit();
    analysisThread_.wait();
    analysisRunner_.reset();
    analysisInProgress_ = false;
    emit analysisInProgressChanged();
}

} // namespace bv
