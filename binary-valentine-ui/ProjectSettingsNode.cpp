#include "ProjectSettingsNode.h"

namespace bv
{

ProjectSettingsNode::ProjectSettingsNode(ProjectTreeNode* parent)
    : ProjectTreeNode(parent, ProjectNodeTypeName::ProjectSettings)
{}

int ProjectSettingsNode::getThreadCount() const
{
    return threadCount_;
}

void ProjectSettingsNode::setThreadCount(int threadCount)
{
    if (threadCount == threadCount_)
        return;

    if (threadCount <= 0)
        threadCount = 1;

    threadCount_ = threadCount;
    emit changesMade();
    emit threadCountChanged();
}

void ProjectSettingsNode::setUseCustomThreadCount(bool enable)
{
    if (useCustomThreadCount_ != enable)
    {
        useCustomThreadCount_ = enable;
        emit changesMade();
        emit useCustomThreadCountChanged();
    }
}

const QString& ProjectSettingsNode::getRootPath() const
{
    return rootPath_;
}

void ProjectSettingsNode::setRootPath(QString path)
{
    if (path == rootPath_)
        return;

    //TODO: validate? check that absolute
    rootPath_ = std::move(path);

    emit changesMade();
    emit rootPathChanged();
}

void ProjectSettingsNode::setCombinedAnalysis(bool enable)
{
    if (combinedAnalysis_ != enable)
    {
        combinedAnalysis_ = enable;
        emit changesMade();
        emit combinedAnalysisChanged();
    }
}

bool ProjectSettingsNode::getCombinedAnalysis() const
{
    return combinedAnalysis_;
}

bool ProjectSettingsNode::hasCustomThreadCount() const
{
    return useCustomThreadCount_;
}

ProjectSettingsNode::AnalysisLimitType ProjectSettingsNode::getAnalysisLimitType() const
{
    return analysisLimitType_;
}

int ProjectSettingsNode::getTotalFileSizeLimitMb() const
{
    return totalFileSizeLimitMb_;
}

int ProjectSettingsNode::getConcurrentFilesLimit() const
{
    return concurrentFilesLimit_;
}

void ProjectSettingsNode::setAnalysisLimitType(AnalysisLimitType type)
{
    if (analysisLimitType_ != type)
    {
        analysisLimitType_ = type;
        emit changesMade();
        emit analysisLimitTypeChanged();
    }
}

void ProjectSettingsNode::setTotalFileSizeLimitMb(int size)
{
    if (totalFileSizeLimitMb_ != size)
    {
        totalFileSizeLimitMb_ = size;
        emit changesMade();
        emit totalFileSizeLimitChanged();
    }
}

void ProjectSettingsNode::setConcurrentFilesLimit(int limit)
{
    if (concurrentFilesLimit_ != limit)
    {
        concurrentFilesLimit_ = limit;
        emit changesMade();
        emit concurrentFilesLimitChanged();
    }
}

} // namespace bv
