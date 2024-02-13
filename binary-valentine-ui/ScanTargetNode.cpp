#include "ScanTargetNode.h"

#include <cassert>
#include <regex>
#include <utility>

#include "TargetsNode.h"

namespace bv
{

ScanTargetNode::ScanTargetNode(ProjectTreeNode* parent, QString path)
    : ProjectTreeNode(parent, ProjectNodeTypeName::ScanTarget)
    , path_(std::move(path))
{}

QVariant ScanTargetNode::dataImpl(int role) const
{
    if (role == ProjectNodeRole::ProjectNodeNameRole)
        return path_;

    return {};
}

void ScanTargetNode::deleteTarget()
{
    emit changesMade();
    emit nodeDeletionRequested(this);
}

void ScanTargetNode::makeRelative()
{
    emit switchPathType(this, PathType::Relative);
}

void ScanTargetNode::makeAbsolute()
{
    emit switchPathType(this, PathType::Absolute);
}

bool ScanTargetNode::setPath(QString path)
{
    if (path == path_)
        return false;

    assert(dynamic_cast<TargetsNode*>(parentNode()) != nullptr);

    if (!static_cast<TargetsNode*>(parentNode())->changePath(this, path_, std::move(path)))
        return false;

    emit changesMade();
    emit pathChanged();
    return true;
}

QVariantList ScanTargetNode::getPathFilters() const
{
    return pathFilters_;
}

QVariantMap ScanTargetNode::addNewPathFilter()
{
    return addPathFilter(IncludePathWhenMatches, QString{});
}

QVariantMap ScanTargetNode::addPathFilter(PathFilterType type, QString regex)
{
    QVariantMap filter;
    filter[pathFilterTypeKey] = static_cast<int>(type);
    filter[pathFilterRegexKey] = regex;
    pathFilters_.emplaceBack(filter);
    emit changesMade();
    return filter;
}

void ScanTargetNode::deletePathFilter(int index)
{
    assert(index >= 0 && index < pathFilters_.size());
    pathFilters_.remove(index);
    emit changesMade();
}

void ScanTargetNode::setPathFilterType(int index, PathFilterType type)
{
    assert(index >= 0 && index < pathFilters_.size());
    auto map = pathFilters_.at(index).toMap();
    map[pathFilterTypeKey] = static_cast<int>(type);
    pathFilters_[index] = std::move(map);
    emit changesMade();
}

bool ScanTargetNode::setPathFilterRegex(int index, QString regex)
{
    try
    {
        [[maybe_unused]] std::regex r(regex.toStdString());
    }
    catch (const std::exception&)
    {
        return false;
    }

    assert(index >= 0 && index < pathFilters_.size());
    auto map = pathFilters_.at(index).toMap();
    map[pathFilterRegexKey] = std::move(regex);
    pathFilters_[index] = std::move(map);
    emit changesMade();
    return true;
}

void ScanTargetNode::addRuleSelector()
{
    emit changesMade();
    emit ruleSelectorRequested(this);
}

void ScanTargetNode::setRecursive(bool recursive)
{
    if (recursive_ != recursive)
    {
        recursive_ = recursive;
        emit changesMade();
        emit isRecursiveChanged();
    }
}

} // namespace bv
