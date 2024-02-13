#pragma once

#include <memory>
#include <span>

#include <QVariantList>

#include "RuleSelectorNode.h"

namespace bv
{

class InMemoryReportOutput;

class AnalyzedFileTracker final
{
public:
    AnalyzedFileTracker();
    ~AnalyzedFileTracker();

    void registerFile(const std::shared_ptr<const InMemoryReportOutput>& file);

    QVariantList getNewlyAddedFiles();
    [[nodiscard]]
    QVariantList getAllFiles(RuleSelectorNode::ReportCategories categories,
                             RuleSelectorNode::ReportLevels levels);
    [[nodiscard]]
    QVariantList getReports(int fileIndex,
                            RuleSelectorNode::ReportCategories categories,
                            RuleSelectorNode::ReportLevels levels);

private:
    QVariantList produceReport(
        const std::span<const std::shared_ptr<const InMemoryReportOutput>> reportOutputs,
        RuleSelectorNode::ReportCategories categories,
        RuleSelectorNode::ReportLevels levels);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} //namespace bv
