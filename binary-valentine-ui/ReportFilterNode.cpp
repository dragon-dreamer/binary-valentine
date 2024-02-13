#include "ReportFilterNode.h"

#include <exception>
#include <regex>
#include <utility>

#include "BinaryValentineContext.h"

namespace bv
{

ReportFilterNode::ReportFilterNode(ProjectTreeNode* parent, QString reportUid)
    : ProjectTreeNode(parent, ProjectNodeTypeName::ReportFilter)
    , reportUid_(std::move(reportUid))
{
    const auto& reports = BinaryValentineContext{}.getAllAvailableReportsWithDescriptionArgs();
    reportInfo_ = reports.find(reportUid_)->toMap();

    const QString& reportDescription = reportInfo_.find("description")->toString();
    auto matches = BinaryValentineContext::getDescriptionArgRegex()
                           .globalMatch(reportDescription);
    while (matches.hasNext())
    {
        QRegularExpressionMatch match = matches.next();
        descriptionArgs_.append(match.captured(1));
    }
}

QVariant ReportFilterNode::dataImpl(int role) const
{
    if (role == ProjectNodeRole::ProjectNodeNameRole)
    {
        if (!reportUid_.isEmpty())
            return reportUid_;

        return {};
    }

    return {};
}

const QString& ReportFilterNode::getReportUid() const
{
    return reportUid_;
}

void ReportFilterNode::deleteFilter()
{
    emit changesMade();
    emit nodeDeletionRequested(this);
}

bool ReportFilterNode::setRegex(QString argName, QString regex)
{
    if (!descriptionArgs_.contains(argName))
        return false;

    try
    {
        [[maybe_unused]] std::regex r(regex.toStdString());
    }
    catch (const std::exception&)
    {
        return false;
    }

    regexes_[std::move(argName)] = std::move(regex);
    emit changesMade();
    return true;
}

QString ReportFilterNode::getRegex(QString argName) const
{
    const auto it = regexes_.find(argName);
    if (it == regexes_.end())
        return {};

    return it.value();
}

void ReportFilterNode::setFilterMode(ReportFilterMode mode)
{
    if (filterMode_ != mode)
    {
        filterMode_ = mode;
        emit changesMade();
        emit filterModeChanged();
    }
}

void ReportFilterNode::setAggregationMode(ReportFilterAggregationMode mode)
{
    if (filterAggregationMode_ != mode)
    {
        filterAggregationMode_ = mode;
        emit changesMade();
        emit filterAggregationModeChanged();
    }
}

} // namespace bv
