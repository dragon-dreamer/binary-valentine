#include "AnalysisProgress.h"

#include <utility>

namespace bv
{

AnalysisProgress::AnalysisProgress(
    int totalRead,
    int totalAnalyzed,
    QString currentAnalyzedTargetPath,
    ProgressState progressState)
    : isValid_(true)
    , totalRead_(totalRead)
    , totalAnalyzed_(totalAnalyzed)
    , currentAnalyzedTargetPath_(std::move(currentAnalyzedTargetPath))
    , progressState_(progressState)
{
}

void AnalysisProgress::setNewFiles(QVariantList newFiles)
{
    newFiles_ = std::move(newFiles);
}

} //namespace bv
