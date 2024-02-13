#pragma once

#include <cstddef>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

#include <QVariant>
#include <QObject>
#include <QQmlEngine>
#include <qnamespace.h>
#include <QtQmlIntegration>

namespace bv
{

struct ProjectNodeTypeName final
{
    ProjectNodeTypeName() = delete;
    static constexpr QLatin1StringView ProjectRoot{"project_root"};
    static constexpr QLatin1StringView ProjectSettings{"project_settings"};
    static constexpr QLatin1StringView RuleSelector{"rule_selector"};
    static constexpr QLatin1StringView ReportFilter{"report_filter"};
    static constexpr QLatin1StringView Targets{"targets"};
    static constexpr QLatin1StringView ScanTarget{"scan_target"};
    static constexpr QLatin1StringView ReportOutputs{"report_outputs"};
    static constexpr QLatin1StringView ScanRunner{"scan_runner"};
};

struct ProjectNodeRole final
{
    ProjectNodeRole() = delete;
    enum Value
    {
        ProjectNodeTypeNameRole = Qt::UserRole + 1,
        ProjectNodeNameRole = Qt::UserRole + 2,
    };
};

class ProjectTreeNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString nodeType READ getNodeType CONSTANT);

public:
    virtual ~ProjectTreeNode() = default;

    ProjectTreeNode(ProjectTreeNode* parent, QLatin1StringView role);

    template<typename Child, typename... Args>
    Child* appendChild(Args&&... args)
    {
        return appendChildAt<Child>(children_.size(), std::forward<Args>(args)...);
    }

    template<typename Child, typename... Args>
    Child* appendChildAt(std::size_t index, Args&&... args)
    {
        auto child = static_cast<Child*>(
            children_.insert(children_.begin() + index,
                             std::make_unique<Child>(
                                 this,
                                 std::forward<Args>(args)...))->get());

        QQmlEngine::setObjectOwnership(child, QQmlEngine::ObjectOwnership::CppOwnership);
        return child;
    }

    template<typename Child, typename ArgsRange>
    auto appendChildrenAt(std::size_t index, ArgsRange&& r)
    {
        std::vector<std::unique_ptr<Child>> children;
        children.reserve(std::ranges::size(r));
        for (auto it = std::ranges::begin(r); it != std::ranges::end(r); ++it)
        {
            auto* child = children.emplace_back(std::make_unique<Child>(this, *it)).get();
            QQmlEngine::setObjectOwnership(child, QQmlEngine::ObjectOwnership::CppOwnership);
        }

        auto firstIt = children_.insert(children_.begin() + index,
                                        std::move_iterator(children.begin()),
                                        std::move_iterator(children.end()));
        return std::pair(firstIt, firstIt + children.size());
    }

    void appendExistingChildAt(std::size_t index, std::unique_ptr<ProjectTreeNode> child)
    {
        children_.insert(children_.begin() + index, std::move(child));
    }

    [[nodiscard]]
    ProjectTreeNode* childAt(std::size_t row);
    [[nodiscard]]
    const ProjectTreeNode* childAt(std::size_t row) const;

    std::unique_ptr<ProjectTreeNode> removeChildAt(std::size_t row);

    [[nodiscard]]
    std::size_t childCount() const;

    [[nodiscard]]
    ProjectTreeNode* parentNode() noexcept;
    [[nodiscard]]
    const ProjectTreeNode* parentNode() const noexcept;

    [[nodiscard]]
    std::size_t row() const;

    [[nodiscard]]
    QVariant data(int role) const;

    [[nodiscard]]
    QString getNodeType() const;

signals:
    void nodeDeletionRequested(ProjectTreeNode* node);
    void changesMade();

protected:
    virtual QVariant dataImpl(int role) const;

private:
    std::size_t indexOfChild(const ProjectTreeNode* child) const;

private:
    ProjectTreeNode* parent_{};
    std::vector<std::unique_ptr<ProjectTreeNode>> children_;
    QLatin1StringView role_;
};
} //namespace bv
