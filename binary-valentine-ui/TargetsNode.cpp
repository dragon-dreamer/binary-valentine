#include "TargetsNode.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <utility>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ranked_index.hpp>

#include "ScanTargetNode.h"

namespace bv
{

struct TargetsNode::Impl
{
public:
    using TargetsContainer = boost::multi_index_container<
        QString,
        boost::multi_index::indexed_by<
            boost::multi_index::ranked_unique<
                    boost::multi_index::identity<QString>
                >
            >
        >;

public:
    TargetsContainer targetsIndex;
};

TargetsNode::TargetsNode(ProjectTreeNode* parent)
    : ProjectTreeNode(parent, ProjectNodeTypeName::Targets)
    , impl_(std::make_unique<Impl>())
{}

TargetsNode::~TargetsNode() = default;

int TargetsNode::addTargets(QStringList paths)
{
    return addTargets(std::move(paths), true);
}

void TargetsNode::appendTargetsInOrder(std::span<QString> paths)
{
    if (paths.empty())
        return;

    TargetsNode::Impl::TargetsContainer& index = impl_->targetsIndex;

    assert(childCount() == 0);

    emit beforeTargetsAdded(this, 0, paths.size() - 1);

    for (auto& path : paths)
    {
        [[maybe_unused]] bool inserted = index.emplace(path).second;
        assert(inserted);

        appendChild<ScanTargetNode>(std::move(path));
    }

    emit targetsAdded(this, 0, paths.size() - 1);
}

int TargetsNode::addTargets(QStringList paths, bool notifyTargetSelected)
{
    int addedCount = 0;
    TargetsNode::Impl::TargetsContainer& index = impl_->targetsIndex;

    std::ranges::sort(paths);
    auto uniqueEnd = std::ranges::unique(paths);
    paths.erase(uniqueEnd.begin(), uniqueEnd.end());
    std::size_t firstAddedNode = (std::numeric_limits<std::size_t>::max)();
    for (std::size_t i = 0; i != paths.size();)
    {
        auto [fromIt, toIt] = index.equal_range(paths[i]);
        if (fromIt != toIt)
        {
            ++i;
            continue;
        }

        std::size_t to = i + 1;
        if (toIt != index.end())
        {
            while (to != paths.size() && paths[to] < *toIt)
                ++to;
        }
        else
        {
            to = paths.size();
        }

        const auto fromChildIndex = index.rank(fromIt);
        const auto toChildIndex = fromChildIndex + to - i - 1;
        emit beforeTargetsAdded(this, fromChildIndex, toChildIndex);
        addedCount += toChildIndex - fromChildIndex + 1;
        auto [firstIt, lastIt] = appendChildrenAt<ScanTargetNode>(
            fromChildIndex,
            std::ranges::subrange(paths.begin() + i,
                                  paths.begin() + to));
        if (firstAddedNode == (std::numeric_limits<std::size_t>::max)())
            firstAddedNode = fromChildIndex;
        while (i != to)
        {
            index.emplace(std::move(paths[i]));
            ++i;
        }

        while (firstIt != lastIt)
        {
            connect(firstIt->get(), &ProjectTreeNode::nodeDeletionRequested,
                    this, &TargetsNode::onNodeDeletionRequested);
            ++firstIt;
        }


        emit targetsAdded(this, fromChildIndex, toChildIndex);
    }

    if (notifyTargetSelected)
    {
        if (firstAddedNode != (std::numeric_limits<std::size_t>::max)())
        {
            emit targetSelected(static_cast<ScanTargetNode*>(
                                    childAt(firstAddedNode)), firstAddedNode);
        }
    }

    if (addedCount)
        emit changesMade();

    return addedCount;
}

void TargetsNode::onNodeDeletionRequested(ProjectTreeNode* node)
{
    const auto& targetNode = *static_cast<const ScanTargetNode*>(node);
    TargetsNode::Impl::TargetsContainer& index = impl_->targetsIndex;
    index.erase(targetNode.getPath());
}

bool TargetsNode::changePath(ScanTargetNode* target, QString& before, QString&& after)
{
    TargetsNode::Impl::TargetsContainer& index = impl_->targetsIndex;
    if (index.contains(after))
        return false;

    const auto it = index.find(before);
    assert(it != index.end());
    const auto rankBefore = index.rank(it);

    auto newIt = index.emplace(after).first;
    const auto rankAfterForEvent = index.rank(newIt);
    index.erase(before);
    const auto rankAfterForVector = index.rank(newIt);

    if (rankBefore == rankAfterForVector)
    {
        before = std::move(after);
        emit pathRenamed(target);
        return true;
    }

    emit beforeTargetMoved(this, rankBefore, rankAfterForEvent);
    auto child = removeChildAt(rankBefore);
    appendExistingChildAt(rankAfterForVector, std::move(child));
    emit afterTargetMoved(this, rankBefore, rankAfterForEvent);
    before = std::move(after);
    emit pathRenamed(target);
    return true;
}

} // namespace bv
