#include "RuleSelectorNode.h"

#include <utility>

#include "ImmutableContext.h"
#include "ReportFilterNode.h"

namespace bv {

const QVariantMap& RuleSelectorNode::getSelectedReports() const
{
    return selectedReports_;
}

void RuleSelectorNode::setSelectedReports(QVariantMap reports)
{
    if (reports == selectedReports_)
        return;

    selectedReports_ = std::move(reports);

    emit changesMade();
    emit selectedReportsChanged();
}

void RuleSelectorNode::selectReport(QString report, bool enable)
{
    if (enable)
        selectedReports_[std::move(report)] = true;
    else
        selectedReports_.remove(report);

    emit changesMade();
    emit selectedReportsChanged();
}

namespace
{
enum class ReportAction
{
    Select,
    Deselect,
    Toggle
};

template<typename Reports>
void selectReportsImpl(Reports&& reports, ReportAction action, QVariantMap& selectedReports)
{
    for (const auto& report : reports)
    {
        QLatin1StringView key(report.get_string_uid());
        switch (action)
        {
        case ReportAction::Select:
            selectedReports[key] = true;
            break;
        case ReportAction::Deselect:
            selectedReports.remove(key);
            break;
        case ReportAction::Toggle:
            if (selectedReports.contains(key))
                selectedReports.remove(key);
            else
                selectedReports[key] = true;
            break;
        }
    }
}

void selectReportsImpl(ReportAction action, QVariantMap& selectedReports)
{
    selectReportsImpl(ImmutableContext::getImmutableContext().get_rules().get_reports(),
                      action, selectedReports);
    selectReportsImpl(ImmutableContext::getImmutableContext().get_combined_rules().get_reports(),
                      action, selectedReports);
}
} //namespace

void RuleSelectorNode::selectAllReports(bool enable)
{
    const auto action = enable ? ReportAction::Select : ReportAction::Deselect;
    selectReportsImpl(action, selectedReports_);
    emit changesMade();
    emit selectedReportsChanged();
}

void RuleSelectorNode::toggleAllReports()
{
    selectReportsImpl(ReportAction::Toggle, selectedReports_);
    emit changesMade();
    emit selectedReportsChanged();
}

void RuleSelectorNode::addReportFilter(QString report)
{
    const auto children = childCount();
    for (std::size_t i = 0; i != children; ++i)
    {
        auto* currentChild = static_cast<ReportFilterNode*>(childAt(i));
        const auto& currentReport = currentChild->getReportUid();
        if (currentReport == report)
        {
            emit reportFilterSelected(currentChild, i);
            return;
        }
        if (currentReport > report)
        {
            emit beforeReportFilterAdded(this, i);
            auto* child = this->appendChildAt<ReportFilterNode>(i, std::move(report));
            emit reportFilterAdded(child, i);
            emit reportFilterSelected(child, i);
            return;
        }
    }

    emit beforeReportFilterAdded(this, children);
    auto* child = this->appendChild<ReportFilterNode>(std::move(report));
    emit reportFilterAdded(child, children);
    emit reportFilterSelected(child, children);
    emit changesMade();
}

ReportFilterNode* RuleSelectorNode::addReportInOrder(QString uid)
{
    const auto children = childCount();
    emit beforeReportFilterAdded(this, children);
    auto* result = this->appendChild<ReportFilterNode>(std::move(uid));
    emit reportFilterAdded(result, children);
    return result;
}

RuleSelectorNode::RuleSelectorNode(ProjectTreeNode* parent, bool isGlobal)
    : ProjectTreeNode(parent, ProjectNodeTypeName::RuleSelector)
    , isGlobal_(isGlobal)
{
    selectReportsImpl(ReportAction::Select, selectedReports_);
}

QVariant RuleSelectorNode::dataImpl(int role) const
{
    if (role == ProjectNodeRole::ProjectNodeNameRole)
        return isGlobal_ ? QString("1") : QString{};

    return {};
}

void RuleSelectorNode::deleteSelector()
{
    emit changesMade();
    emit nodeDeletionRequested(this);
}

RuleSelectorNode::ReportSelectionPolicy RuleSelectorNode::getReportSelectionPolicy() const noexcept
{
    return reportSelectionPolicy_;
}

RuleSelectorNode::ReportLevels RuleSelectorNode::getReportLevels() const noexcept
{
    return reportLevels_;
}

RuleSelectorNode::ReportCategories RuleSelectorNode::getReportCategories() const noexcept
{
    return reportCategories_;
}

void RuleSelectorNode::setSelectedLevels(ReportLevels levels)
{
    if (reportLevels_ != levels)
    {
        reportLevels_ = levels;
        emit changesMade();
        emit reportLevelsChanged();
    }
}

void RuleSelectorNode::setSelectedCategories(ReportCategories categories)
{
    if (reportCategories_ != categories)
    {
        reportCategories_ = categories;
        emit changesMade();
        emit reportCategoriesChanged();
    }
}

void RuleSelectorNode::setSelectionPolicy(ReportSelectionPolicy policy)
{
    if (reportSelectionPolicy_ != policy)
    {
        reportSelectionPolicy_ = policy;
        emit changesMade();
        emit reportSelectionPolicyChanged();
    }
}

} // namespace bv
