#pragma once

#include <memory>

#include <QObject>
#include <QString>
#include <QThread>
#include <QtQmlIntegration>

#include "AnalysisProgress.h"
#include "ProjectTreeNode.h"
#include "ProjectTreeNodes.h"
#include "RuleSelectorNode.h"

namespace bv
{

class AnalyzedFileTracker;

namespace output::format
{
class analysis_state;
} //namespace output::format

class AnalysisRunner;
class ProgressReport;

class ScanRunnerNode final : public ProjectTreeNode
{
private:
    Q_OBJECT
    QML_NAMED_ELEMENT(ScanRunner);
    QML_UNCREATABLE("Cannot be created from QML");

public:
    Q_PROPERTY(bool analysisInProgress MEMBER analysisInProgress_ NOTIFY analysisInProgressChanged);
    Q_PROPERTY(RuleSelectorNode::ReportCategories reportCategories
                   MEMBER reportCategories_ NOTIFY reportCategoriesChanged);
    Q_PROPERTY(RuleSelectorNode::ReportLevels reportLevels
                   MEMBER reportLevels_ NOTIFY reportLevelsChanged);
    Q_PROPERTY(int selectedFileIndex MEMBER selectedFileIndex_ NOTIFY selectedFileIndexChanged);

signals:
    void analysisInProgressChanged();
    void kickOffAnalysis();
    void reportCategoriesChanged();
    void reportLevelsChanged();
    void selectedFileIndexChanged();

public:
    explicit ScanRunnerNode(ProjectTreeNode* parent, const ProjectTreeNodes& nodes);

public:
    [[nodiscard]]
    Q_INVOKABLE AnalysisProgress* getAnalysisProgress() const;
    [[nodiscard]]
    Q_INVOKABLE QVariantList getFileReport();
    [[nodiscard]]
    Q_INVOKABLE QVariantList getAllFiles();

    [[nodiscard]]
    bool isAnalysisInProgress() const noexcept
    {
        return analysisInProgress_;
    }

public:
    Q_INVOKABLE void startAnalysis();
    Q_INVOKABLE void requestAnalysisStop();
    void analysisComplete(output::format::analysis_state state);

private:
    void runAnalysis();

private:
    bool analysisInProgress_{};
    bool analysisStopRequested_{};
    ProjectTreeNodes nodes_;
    QThread analysisThread_;
    std::shared_ptr<const ProgressReport> progressReport_;
    std::shared_ptr<AnalysisRunner> analysisRunner_;
    std::shared_ptr<AnalyzedFileTracker> fileTracker_;
    RuleSelectorNode::ReportCategories reportCategories_{
        RuleSelectorNode::ReportCategoryConfiguration
        | RuleSelectorNode::ReportCategoryFormat
        | RuleSelectorNode::ReportCategoryOptimization
        | RuleSelectorNode::ReportCategorySecurity
        | RuleSelectorNode::ReportCategorySystem
    };
    RuleSelectorNode::ReportLevels reportLevels_{
        RuleSelectorNode::ReportLevelCritical
        | RuleSelectorNode::ReportLevelError
        | RuleSelectorNode::ReportLevelWarning
        | RuleSelectorNode::ReportLevelInfo
    };
    int selectedFileIndex_{};
};

} // namespace bv
