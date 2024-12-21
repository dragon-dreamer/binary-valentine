#pragma once

#include <atomic>
#include <shared_mutex>
#include <string>

#include "binary_valentine/progress/progress_report_interface.h"

#include "AnalysisProgress.h"

namespace bv
{

class ProgressReport final : public progress::progress_report_interface
{
public:
    [[nodiscard]]
    int getTotalRead() const noexcept;
    [[nodiscard]]
    int getTotalAnalyzed() const noexcept;
    [[nodiscard]]
    std::string getCurrentAnalyzedTargetPath() const;
    [[nodiscard]]
    AnalysisProgress::ProgressState getCurrentState() const;

private:
    virtual void report_progress(
        const std::shared_ptr<const core::subject_entity_interface>& entity,
        progress::progress_state state) noexcept override;

private:
    mutable std::shared_mutex mutex_;
    std::string currentAnalyzedTargetPath_;
    std::atomic<int> totalRead_{};
    std::atomic<int> totalAnalyzed_{};
    std::atomic<int> currentState_{ static_cast<int>(
        AnalysisProgress::LoadingSharedDependencies)
    };
};

} // namespace bv
