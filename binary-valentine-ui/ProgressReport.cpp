#include "ProgressReport.h"

#include "binary_valentine/core/subject_entity_interface.h"

namespace bv
{

void ProgressReport::report_progress(
    const std::shared_ptr<const core::subject_entity_interface>& entity,
    progress::progress_state state) noexcept
{
    if (state == progress::progress_state::combined_analysis_started)
    {
        currentState_.store(static_cast<int>(AnalysisProgress::CombinedAnalysis),
                            std::memory_order_relaxed);
        return;
    }

    if (state == progress::progress_state::combined_analysis_completed)
    {
        currentState_.store(static_cast<int>(AnalysisProgress::WritingReports),
                            std::memory_order_relaxed);
        return;
    }

    if (state == progress::progress_state::load_started)
    {
        totalRead_.fetch_add(1u, std::memory_order_relaxed);
        return;
    }

    if (state == progress::progress_state::analysis_completed)
    {
        totalAnalyzed_.fetch_add(1u, std::memory_order_relaxed);

        std::unique_lock lock(mutex_);
        currentAnalyzedTargetPath_ = entity->get_utf8_path();
    }
}

int ProgressReport::getTotalRead() const noexcept
{
    return totalRead_.load(std::memory_order_relaxed);
}

int ProgressReport::getTotalAnalyzed() const noexcept
{
    return totalAnalyzed_.load(std::memory_order_relaxed);
}

std::string ProgressReport::getCurrentAnalyzedTargetPath() const
{
    std::shared_lock lock(mutex_);
    return currentAnalyzedTargetPath_;
}

AnalysisProgress::ProgressState ProgressReport::getCurrentState() const
{
    return static_cast<AnalysisProgress::ProgressState>(
        currentState_.load(std::memory_order_relaxed));
}

} // namespace bv
