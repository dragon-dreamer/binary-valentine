#pragma once

#include <cstddef>
#include <memory>

#include <QAbstractItemModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QQmlEngine>
#include <QtQml/qqmlregistration.h>

#include "ProjectRootNode.h"
#include "ScanTargetNode.h"

namespace bv
{

class ProjectSettingsNode;
class RuleSelectorNode;
class ReportFilterNode;
class ReportOutputsNode;
class ScanRunnerNode;
class TargetsNode;
struct ProjectTreeNodes;

class ProjectModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    Q_PROPERTY(bool analysisInProgress
                   MEMBER analysisInProgress_ NOTIFY analysisInProgressChanged);
    Q_PROPERTY(QString projectPath MEMBER projectPath_ NOTIFY projectPathChanged);
    Q_PROPERTY(bool hasChanges MEMBER hasChanges_ NOTIFY hasChangesChanged);

public:
    ProjectModel();

    [[nodiscard]]
    Q_INVOKABLE QObject* getNode(const QModelIndex& index);
    Q_INVOKABLE QString saveToFile(QString path);

    virtual QHash<int, QByteArray> roleNames() const override;
    virtual QVariant data(const QModelIndex& index, int role) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QModelIndex index(int row, int column,
                              const QModelIndex& parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex& index) const override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;

public:
    Q_INVOKABLE void setPathsToScan(QStringList paths);
    Q_INVOKABLE void requestAnalysisStop();

public:
    void setProjectPath(QString path);
    void resetChanges();

public:
    [[nodiscard]]
    const ProjectSettingsNode& getProjectSettings() const noexcept
    {
        return *projectSettings_;
    }
    [[nodiscard]]
    const RuleSelectorNode& getGlobalSelector() const noexcept
    {
        return *globalSelector_;
    }
    [[nodiscard]]
    const TargetsNode& getTargets() const noexcept
    {
        return *targets_;
    }
    [[nodiscard]]
    const ReportOutputsNode& getReportOutputs() const noexcept
    {
        return *reportOutputs_;
    }

    [[nodiscard]]
    ProjectSettingsNode& getProjectSettings() noexcept
    {
        return *projectSettings_;
    }
    [[nodiscard]]
    RuleSelectorNode& getGlobalSelector() noexcept
    {
        return *globalSelector_;
    }
    [[nodiscard]]
    TargetsNode& getTargets() noexcept
    {
        return *targets_;
    }
    [[nodiscard]]
    ReportOutputsNode& getReportOutputs() noexcept
    {
        return *reportOutputs_;
    }

signals:
    void rowSelected(QModelIndex index, QModelIndex parentIndex);
    void analysisInProgressChanged();
    void projectPathChanged();
    void hasChangesChanged();

private:
    [[nodiscard]]
    ProjectTreeNodes getNodes() const;

private:
    void reportFilterAdded(ReportFilterNode* filter, int childIndex);
    void reportFilterSelected(ReportFilterNode* filter, int childIndex);
    void beforeReportFilterAdded(RuleSelectorNode* parent, int childIndex);
    std::unique_ptr<ProjectTreeNode> nodeDeletionRequested(ProjectTreeNode* node);
    void beforeTargetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex);
    void targetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex);
    void targetSelected(ScanTargetNode* target, int childIndex);
    void switchPathType(ScanTargetNode* target, ScanTargetNode::PathType type);
    void beforeTargetMoved(TargetsNode* parent, std::size_t rankBefore, std::size_t rankAfter);
    void afterTargetMoved(TargetsNode* parent, std::size_t rankBefore, std::size_t rankAfter);
    void pathRenamed(ScanTargetNode* target);
    void targetRuleSelectorRequested(ScanTargetNode* target);
    void onAnalysisInProgressChanged();
    void onChangesMade();

private:
    ProjectRootNode root_;
    ProjectSettingsNode* projectSettings_{};
    RuleSelectorNode* globalSelector_{};
    TargetsNode* targets_{};
    ReportOutputsNode* reportOutputs_{};
    ScanRunnerNode* scanRunner_{};
    bool analysisInProgress_{};
    QString projectPath_;
    bool hasChanges_{};
};

} //namespace bv
