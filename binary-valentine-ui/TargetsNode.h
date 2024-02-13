#pragma once

#include <cstddef>
#include <memory>
#include <span>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"
#include "ScanTargetNode.h"

namespace bv
{
struct TargetInfo final
{
    QString path;
    bool recursive{};
};

class TargetsNode final : public ProjectTreeNode
{
private:
    Q_OBJECT
    QML_NAMED_ELEMENT(Targets);
    QML_UNCREATABLE("Cannot be created from QML");

public:
    explicit TargetsNode(ProjectTreeNode* parent);
    virtual ~TargetsNode() override;

signals:
    void beforeTargetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex);
    void targetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex);
    void targetSelected(ScanTargetNode* target, int childIndex);
    void beforeTargetMoved(TargetsNode* parent, std::size_t rankBefore, std::size_t rankAfter);
    void afterTargetMoved(TargetsNode* parent, std::size_t rankBefore, std::size_t rankAfter);
    void pathRenamed(ScanTargetNode* target);

public:
    Q_INVOKABLE int addTargets(QStringList paths);
    int addTargets(QStringList paths, bool notifyTargetSelected);
    bool changePath(ScanTargetNode* target, QString& before, QString&& after);

    void appendTargetsInOrder(std::span<QString> paths);
    void onNodeDeletionRequested(ProjectTreeNode* node);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};
} // namespace bv
