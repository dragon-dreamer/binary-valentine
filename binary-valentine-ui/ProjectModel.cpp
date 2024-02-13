#include "ProjectModel.h"

#include <initializer_list>
#include <utility>

#ifdef _DEBUG
#   include <QAbstractItemModelTester>
#endif //_DEBUG
#include <QFileInfo>

#include "AnalysisPlanXmlSaver.h"
#include "ExceptionFormatter.h"
#include "ModelToAnalysisPlanConverter.h"
#include "ProjectSettingsNode.h"
#include "ProjectTreeNodes.h"
#include "RuleSelectorNode.h"
#include "ReportFilterNode.h"
#include "ReportOutputsNode.h"
#include "ScanRunnerNode.h"
#include "TargetsNode.h"

namespace bv
{

ProjectModel::ProjectModel()
{
    projectSettings_ = root_.appendChild<ProjectSettingsNode>();
    globalSelector_ = root_.appendChild<RuleSelectorNode>(true);
    targets_ = root_.appendChild<TargetsNode>();
    reportOutputs_ = root_.appendChild<ReportOutputsNode>();
    scanRunner_ = root_.appendChild<ScanRunnerNode>(getNodes());

    connect(globalSelector_, &RuleSelectorNode::beforeReportFilterAdded,
            this, &ProjectModel::beforeReportFilterAdded);
    connect(globalSelector_, &RuleSelectorNode::reportFilterAdded,
            this, &ProjectModel::reportFilterAdded);
    connect(globalSelector_, &RuleSelectorNode::reportFilterSelected,
            this, &ProjectModel::reportFilterSelected);
    connect(targets_, &TargetsNode::beforeTargetsAdded,
            this, &ProjectModel::beforeTargetsAdded);
    connect(targets_, &TargetsNode::targetsAdded,
            this, &ProjectModel::targetsAdded);
    connect(targets_, &TargetsNode::targetSelected,
            this, &ProjectModel::targetSelected);
    connect(targets_, &TargetsNode::beforeTargetMoved,
            this, &ProjectModel::beforeTargetMoved);
    connect(targets_, &TargetsNode::afterTargetMoved,
            this, &ProjectModel::afterTargetMoved);
    connect(targets_, &TargetsNode::pathRenamed,
            this, &ProjectModel::pathRenamed);
    connect(scanRunner_, &ScanRunnerNode::analysisInProgressChanged,
            this, &ProjectModel::onAnalysisInProgressChanged);

    for (ProjectTreeNode* node : std::initializer_list<ProjectTreeNode*>
         {projectSettings_, globalSelector_, targets_, reportOutputs_})
    {
        connect(node, &ProjectTreeNode::changesMade,
                this, &ProjectModel::onChangesMade);
    }

#ifdef _DEBUG
    new QAbstractItemModelTester(this, QAbstractItemModelTester::FailureReportingMode::Warning);
#endif //_DEBUG
}

void ProjectModel::onChangesMade()
{
    hasChanges_ = true;
    emit hasChangesChanged();
}

ProjectTreeNodes ProjectModel::getNodes() const
{
    return {
        .reportOutputsNode = *reportOutputs_,
        .targetsNode = *targets_,
        .ruleSelectorNode = *globalSelector_,
        .projectSettingsNode = *projectSettings_
    };
}

void ProjectModel::beforeTargetMoved(TargetsNode* parent,
                                     std::size_t rankBefore, std::size_t rankAfter)
{
    const auto parentIndex = createIndex(parent->row(), 0, parent);
    beginMoveRows(parentIndex, rankBefore, rankBefore, parentIndex, rankAfter);
}

void ProjectModel::afterTargetMoved(
    TargetsNode* /* parent */, std::size_t /* rankBefore */, std::size_t /* rankAfter */)
{
    endMoveRows();
}

void ProjectModel::beforeTargetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex)
{
    beginInsertRows(createIndex(parent->row(), 0, parent), fromChildIndex, toChildIndex);
}

void ProjectModel::targetsAdded(TargetsNode* parent, int fromChildIndex, int toChildIndex)
{
    endInsertRows();
    for (int i = fromChildIndex; i <= toChildIndex; ++i)
    {
        auto* child = static_cast<ScanTargetNode*>(parent->childAt(i));
        connect(child, &ScanTargetNode::nodeDeletionRequested,
                this, &ProjectModel::nodeDeletionRequested);
        connect(child, &ScanTargetNode::switchPathType,
                this, &ProjectModel::switchPathType);
        connect(child, &ScanTargetNode::ruleSelectorRequested,
                this, &ProjectModel::targetRuleSelectorRequested);
        connect(child, &ScanTargetNode::changesMade,
                this, &ProjectModel::onChangesMade);
    }
}

void ProjectModel::targetRuleSelectorRequested(ScanTargetNode* target)
{
    if (!target->childCount())
    {
        beginInsertRows(createIndex(target->row(), 0, target), 0, 0);

        auto* selector = static_cast<RuleSelectorNode*>(
            target->appendChild<RuleSelectorNode>(false));
        endInsertRows();

        connect(selector, &RuleSelectorNode::beforeReportFilterAdded,
                this, &ProjectModel::beforeReportFilterAdded);
        connect(selector, &RuleSelectorNode::reportFilterAdded,
                this, &ProjectModel::reportFilterAdded);
        connect(selector, &RuleSelectorNode::reportFilterSelected,
                this, &ProjectModel::reportFilterSelected);
        connect(selector, &RuleSelectorNode::nodeDeletionRequested,
                this, &ProjectModel::nodeDeletionRequested);
        connect(selector, &RuleSelectorNode::changesMade,
                this, &ProjectModel::onChangesMade);
    }

    auto* selector = target->childAt(0);
    auto index = createIndex(selector->row(), 0, selector);
    emit rowSelected(index, parent(index));
}

void ProjectModel::switchPathType(ScanTargetNode* target, ScanTargetNode::PathType type)
{
    ScanTargetNode::PathType currentType{};
    try
    {
        QFileInfo info(target->getPath());
        currentType = info.isAbsolute()
                          ? ScanTargetNode::PathType::Absolute
                          : ScanTargetNode::PathType::Relative;
    }
    catch (...)
    {
        return;
    }

    if (currentType == type)
        return;

    const QString& rootPath = projectSettings_->getRootPath();
    if (rootPath.isEmpty())
        return;

    try
    {
        QFileInfo info(rootPath);
        if (!info.isAbsolute())
            return;
    }
    catch (...)
    {
        return;
    }

    if (type == ScanTargetNode::PathType::Absolute)
    {
        if (!target->getPath().startsWith(rootPath))
        {
            QString fullPath = rootPath;
            if (!fullPath.endsWith('/') && !fullPath.endsWith('\\'))
                fullPath += '/';
            fullPath += target->getPath();
            target->setPath(std::move(fullPath));
        }
    }
    else
    {
        if (target->getPath().startsWith(rootPath))
        {
            QString relativePath = target->getPath().right(
                target->getPath().size() - rootPath.size());
            while (relativePath.startsWith('/') || relativePath.startsWith('\\'))
                relativePath = relativePath.right(relativePath.size() - 1);
            target->setPath(std::move(relativePath));
        }
    }
}

void ProjectModel::pathRenamed(ScanTargetNode* target)
{
    const auto targetIndex = createIndex(target->row(), 0, target);
    emit dataChanged(targetIndex, targetIndex,
                     { static_cast<int>(ProjectNodeRole::ProjectNodeNameRole) });
}

void ProjectModel::reportFilterAdded(ReportFilterNode* filter, int /* childIndex */)
{
    connect(filter, &ReportFilterNode::nodeDeletionRequested,
            this, &ProjectModel::nodeDeletionRequested);
    connect(filter, &ReportFilterNode::changesMade,
            this, &ProjectModel::onChangesMade);
    endInsertRows();
}

std::unique_ptr<ProjectTreeNode> ProjectModel::nodeDeletionRequested(ProjectTreeNode* node)
{
    const auto nodeRow = node->row();
    const auto parentNode = node->parentNode();
    auto parentIndex = createIndex(node->parentNode()->row(), 0, node->parentNode());
    beginRemoveRows(parentIndex, nodeRow, nodeRow);
    auto result = parentNode->removeChildAt(nodeRow);
    endRemoveRows();
    emit rowSelected(parentIndex, parent(parentIndex));
    return result;
}

void ProjectModel::beforeReportFilterAdded(RuleSelectorNode* parent, int childIndex)
{
    beginInsertRows(createIndex(parent->row(), 0, parent), childIndex, childIndex);
}

void ProjectModel::targetSelected(ScanTargetNode* target, int childIndex)
{
    auto index = createIndex(childIndex, 0, target);
    emit rowSelected(index, parent(index));
}

void ProjectModel::reportFilterSelected(ReportFilterNode* filter, int childIndex)
{
    auto index = createIndex(childIndex, 0, filter);
    emit rowSelected(index, parent(index));
}

QHash<int, QByteArray> ProjectModel::roleNames() const
{
    return {{
                static_cast<int>(ProjectNodeRole::ProjectNodeTypeNameRole),
                "nodeType"
            }, {
                static_cast<int>(ProjectNodeRole::ProjectNodeNameRole),
                "nodeName"
            }};
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    const bv::ProjectTreeNode* item = static_cast<const bv::ProjectTreeNode*>(
        index.internalPointer());

    return item->data(role);
}

Qt::ItemFlags ProjectModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex ProjectModel::index(int row, int column,
                                const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    const bv::ProjectTreeNode* parentItem;

    if (!parent.isValid())
    {
        parentItem = &root_;
    }
    else
    {
        parentItem = static_cast<const bv::ProjectTreeNode*>(
            parent.internalPointer());
    }

    const bv::ProjectTreeNode* childItem = parentItem->childAt(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex ProjectModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return {};

    const bv::ProjectTreeNode* childItem = static_cast<const bv::ProjectTreeNode*>(
        index.internalPointer());
    const bv::ProjectTreeNode* parentItem = childItem->parentNode();
    if (!parentItem || parentItem == &root_)
        return {};

    return createIndex(parentItem->row(), 0, parentItem);
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const bv::ProjectTreeNode* parentItem;
    if (!parent.isValid())
    {
        parentItem = &root_;
    }
    else
    {
        parentItem = static_cast<const bv::ProjectTreeNode*>(
            parent.internalPointer());
    }

    return static_cast<int>(parentItem->childCount());
}

int ProjectModel::columnCount(const QModelIndex& /* parent */) const
{
    return 1;
}

QObject* ProjectModel::getNode(const QModelIndex& index)
{
    if (!index.isValid())
        return projectSettings_;

    bv::ProjectTreeNode* parentItem
        = static_cast<bv::ProjectTreeNode*>(
            index.internalPointer());

    return parentItem;
}

void ProjectModel::setPathsToScan(QStringList paths)
{
    targets_->addTargets(std::move(paths));
    auto index = createIndex(scanRunner_->row(), 0, scanRunner_);
    emit rowSelected(index, parent(index));
}

void ProjectModel::onAnalysisInProgressChanged()
{
    analysisInProgress_ = scanRunner_->isAnalysisInProgress();
    emit analysisInProgressChanged();
}

void ProjectModel::setProjectPath(QString path)
{
    if (projectPath_ != path)
    {
        projectPath_ = std::move(path);
        emit projectPathChanged();
    }
}

QString ProjectModel::saveToFile(QString path)
{
    try
    {
        AnalysisPlanXmlSaver::saveToXml(
            ModelToAnalysisPlanConverter::toPlan(getNodes()), path);
    }
    catch (const std::exception&)
    {
        return ExceptionFormatter::formatException(std::current_exception());
    }

    resetChanges();
    setProjectPath(std::move(path));
    return QString{};
}

void ProjectModel::resetChanges()
{
    if (hasChanges_)
    {
        hasChanges_ = false;
        emit hasChangesChanged();
    }
}

void ProjectModel::requestAnalysisStop()
{
    scanRunner_->requestAnalysisStop();
}

} //namespace bv
