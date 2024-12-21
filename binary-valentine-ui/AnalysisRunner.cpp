#include "AnalysisRunner.h"

#include <exception>
#include <utility>

#include "binary_valentine/analysis/analysis_plan_runner.h"
#include "binary_valentine/output/internal_report_messages.h"

#include "ImmutableContext.h"

namespace bv
{

AnalysisRunner::AnalysisRunner(analysis::shared_context& shared_context,
                               analysis::analysis_plan&& plan,
                               std::shared_ptr<output::common_report_interface> report)
    : shared_context_(shared_context)
    , runner_(std::move(plan),
              ImmutableContext::getImmutableContext(),
              shared_context_,
              ImmutableContext::getLocalizedResources())
    , report_(std::move(report))
{
}

void AnalysisRunner::runAnalysisAndWait()
{
    output::format::analysis_state state{};

    try
    {
        runner_.start();
        runner_.join();
        state = runner_.write_reports(report_.get());
    }
    catch (const std::exception&)
    {
        report_->log_noexcept(
            output::report_level::critical,
            output::reports::analysis_exception,
            output::current_exception_arg());
    }

    emit analysisComplete(std::move(state));
}

void AnalysisRunner::requestInterrupt()
{
    runner_.interrupt();
}

} // namespace bv
