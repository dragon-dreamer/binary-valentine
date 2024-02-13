#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <QVariantMap>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"
#include "ReportFilterNode.h"

namespace bv
{

class RuleSelectorNode final : public ProjectTreeNode
{
public:
    enum ReportSelectionPolicy
    {
        SelectAllReports,
        ExcludeReports,
        IncludeReports
    };
    Q_ENUM(ReportSelectionPolicy);

    enum ReportLevel
    {
        ReportLevelInfo = 1 << 0,
        ReportLevelWarning = 1 << 1,
        ReportLevelError = 1 << 2,
        ReportLevelCritical = 1 << 3
    };
    Q_DECLARE_FLAGS(ReportLevels, ReportLevel);
    Q_FLAG(ReportLevel);

    enum ReportCategory
    {
        ReportCategorySystem = 1 << 0,
        ReportCategoryOptimization = 1 << 1,
        ReportCategorySecurity = 1 << 2,
        ReportCategoryConfiguration = 1 << 3,
        ReportCategoryFormat = 1 << 4
    };
    Q_DECLARE_FLAGS(ReportCategories, ReportCategory);
    Q_FLAG(ReportCategory);

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(RuleSelector);
    QML_UNCREATABLE("Cannot be created from QML");
    Q_PROPERTY(ReportSelectionPolicy reportSelectionPolicy READ getReportSelectionPolicy
                   WRITE setSelectionPolicy NOTIFY reportSelectionPolicyChanged);
    Q_PROPERTY(QVariantMap selectedReports READ getSelectedReports
                   WRITE setSelectedReports NOTIFY selectedReportsChanged);
    Q_PROPERTY(ReportLevels reportLevels READ getReportLevels
                   WRITE setSelectedLevels NOTIFY reportLevelsChanged);
    Q_PROPERTY(ReportCategories reportCategories READ getReportCategories
                   WRITE setSelectedCategories NOTIFY reportCategoriesChanged);
    Q_PROPERTY(bool isGlobal MEMBER isGlobal_ FINAL CONSTANT);

signals:
    void reportSelectionPolicyChanged();
    void selectedReportsChanged();
    void reportLevelsChanged();
    void reportCategoriesChanged();
    void beforeReportFilterAdded(RuleSelectorNode* parent, int childIndex);
    void reportFilterAdded(ReportFilterNode* filter, int childIndex);
    void reportFilterSelected(ReportFilterNode* filter, int childIndex);

public:
    explicit RuleSelectorNode(ProjectTreeNode* parent, bool isGlobal);

public:
    [[nodiscard]]
    const QVariantMap& getSelectedReports() const;
    void setSelectedReports(QVariantMap reports);
    void setSelectedLevels(ReportLevels levels);
    void setSelectedCategories(ReportCategories categories);
    void setSelectionPolicy(ReportSelectionPolicy policy);
    [[nodiscard]]
    ReportFilterNode* addReportInOrder(QString uid);

    Q_INVOKABLE void selectReport(QString report, bool enable);
    Q_INVOKABLE void selectAllReports(bool enable);
    Q_INVOKABLE void toggleAllReports();
    Q_INVOKABLE void addReportFilter(QString report);
    Q_INVOKABLE void deleteSelector();

    [[nodiscard]]
    ReportSelectionPolicy getReportSelectionPolicy() const noexcept;
    [[nodiscard]]
    ReportLevels getReportLevels() const noexcept;
    [[nodiscard]]
    ReportCategories getReportCategories() const noexcept;

private:
    QVariant dataImpl(int role) const override;

private:
    bool isGlobal_{};
    ReportSelectionPolicy reportSelectionPolicy_{SelectAllReports};
    QVariantMap selectedReports_;
    ReportLevels reportLevels_{ReportLevel::ReportLevelInfo
                               | ReportLevel::ReportLevelWarning
                               | ReportLevel::ReportLevelError
                               | ReportLevel::ReportLevelCritical};
    ReportCategories reportCategories_{ReportCategory::ReportCategoryConfiguration
                                       | ReportCategory::ReportCategoryFormat
                                       | ReportCategory::ReportCategoryOptimization
                                       | ReportCategory::ReportCategorySecurity
                                       | ReportCategory::ReportCategorySystem};
};

Q_DECLARE_OPERATORS_FOR_FLAGS(RuleSelectorNode::ReportLevels);
Q_DECLARE_OPERATORS_FOR_FLAGS(RuleSelectorNode::ReportCategories);

} // namespace bv

