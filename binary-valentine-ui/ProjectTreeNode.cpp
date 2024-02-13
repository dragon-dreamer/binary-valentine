#include "ProjectTreeNode.h"

#include <algorithm>
#include <stdexcept>

namespace bv
{

ProjectTreeNode::ProjectTreeNode(ProjectTreeNode* parent, QLatin1StringView role)
    : parent_(parent)
    , role_(role)
{
}

ProjectTreeNode* ProjectTreeNode::childAt(std::size_t row)
{
    return children_.at(row).get();
}

const ProjectTreeNode* ProjectTreeNode::childAt(std::size_t row) const
{
    return children_.at(row).get();
}

std::unique_ptr<ProjectTreeNode> ProjectTreeNode::removeChildAt(std::size_t row)
{
    if (row >= children_.size())
        throw std::out_of_range("No child with row");

    std::unique_ptr<ProjectTreeNode> result = std::move(children_[row]);
    children_.erase(children_.begin() + row);
    return result;
}

std::size_t ProjectTreeNode::childCount() const
{
    return children_.size();
}

ProjectTreeNode* ProjectTreeNode::parentNode() noexcept
{
    return parent_;
}

const ProjectTreeNode* ProjectTreeNode::parentNode() const noexcept
{
    return parent_;
}

std::size_t ProjectTreeNode::row() const
{
    if (!parent_)
        return 0;

    return parent_->indexOfChild(this);
}

std::size_t ProjectTreeNode::indexOfChild(const ProjectTreeNode* child) const
{
    auto it = std::ranges::find_if(children_, [child] (const auto& ptr) {
        return ptr.get() == child;
    });
    if (it == children_.end())
        throw std::runtime_error("No child");
    return std::ranges::distance(children_.begin(), it);
}

QVariant ProjectTreeNode::data(int role) const
{
    if (role == ProjectNodeRole::ProjectNodeTypeNameRole)
        return role_;

    return dataImpl(role);
}

QVariant ProjectTreeNode::dataImpl(int role) const
{
    return {};
}

QString ProjectTreeNode::getNodeType() const
{
    return role_;
}

} // namespace bv
