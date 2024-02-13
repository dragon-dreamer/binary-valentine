#include "ProjectModelLoader.h"

#include <memory>

#include "binary_valentine/analysis/xml_analysis_plan_provider.h"
#include "binary_valentine/core/user_error.h"

#include "AnalysisPlanModelPopulator.h"
#include "ExceptionFormatter.h"
#include "ImmutableContext.h"
#include "QtStdTypeConverter.h"

namespace bv
{

ProjectModel* ProjectModelLoader::createNewProjectModel()
{
    lastError_.clear();
    return new ProjectModel{};
}

ProjectModel* ProjectModelLoader::createNewProjectModelFromProject(QString path)
{
    std::unique_ptr<ProjectModel> model;

    try
    {
        const analysis::xml_analysis_plan_provider provider(
            QtStdTypeConverter::toPath(path, core::user_errc::invalid_project_path),
            ImmutableContext::getImmutableContext(), {
                .allow_empty_targets = true,
                .allow_empty_reports = true
            });

        auto plan = provider.get();
        model = std::make_unique<ProjectModel>();
        AnalysisPlanModelPopulator::populateModel(plan, *model);
        model->setProjectPath(std::move(path));
    }
    catch (const std::exception&)
    {
        model.reset();
        lastError_ = ExceptionFormatter::formatException(std::current_exception());
        return nullptr;
    }

    model->resetChanges();
    return model.release();
}

QString ProjectModelLoader::getLastError() const
{
    return lastError_;
}

} // namespace bv
