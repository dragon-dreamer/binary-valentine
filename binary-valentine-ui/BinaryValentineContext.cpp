#include "BinaryValentineContext.h"

#include "ImmutableContext.h"

#include <algorithm>
#include <vector>

#include "binary_valentine/string/rule_report_resource_helper.h"

namespace bv
{

namespace
{
template<typename Reports>
void appendReports(std::vector<output::rule_report_base>& out, Reports&& reports)
{
    for (auto& report : reports)
        out.emplace_back(report);
}

template<typename... Reports>
std::vector<output::rule_report_base> getSortedReports(Reports&&... reports)
{
    std::vector<output::rule_report_base> sortedReports;
    sortedReports.reserve((... + reports.size()));
    (..., appendReports(sortedReports, reports));
    std::ranges::sort(sortedReports, [](const auto& l, const auto& r) {
        return l.get_string_uid() < r.get_string_uid();
    });
    return sortedReports;
}

template<typename... Reports>
QVariantMap getAvailableReportsImpl(bool includeWithDescriptionArgsOnly, Reports&&... reports)
{
    QVariantMap result;
    const auto sortedReports = getSortedReports(reports...);
    const auto& resources = ImmutableContext::getLocalizedResources();
    const auto& argRegex = BinaryValentineContext::getDescriptionArgRegex();
    for (const output::rule_report_base& report : sortedReports)
    {
        string::rule_report_resource_helper helper(report, resources);
        const auto& description = helper.get_report_description();
        const QLatin1StringView descriptionView(description);
        if (includeWithDescriptionArgsOnly
            && !argRegex.match(descriptionView).hasMatch()) {
            continue;
        }

        const QLatin1StringView uid(report.get_string_uid());

        QVariantMap map;
        map.insert("uid", uid);

        const auto& category = helper.get_report_category();
        map.insert("categoryText", QLatin1StringView(category));

        const auto& level = helper.get_report_level();
        map.insert("levelText", QLatin1StringView(level));

        const auto& name = helper.get_report_uid_name();
        map.insert("name", QLatin1StringView(name));

        map.insert("category", BinaryValentineContext::toReportCategory(
                                   report.get_report_category()));
        map.insert("level", BinaryValentineContext::toReportLevel(
                                report.get_report_level()));
        const auto& ruleName = helper.get_rule_name();
        map.insert("ruleName", QLatin1StringView(ruleName));
        map.insert("description", descriptionView);

        result.insert(uid, QVariant::fromValue(std::move(map)));
    }

    return result;
}
} //namespace

BinaryValentineContext::ReportCategory BinaryValentineContext::toReportCategory(
    output::report_category cat)
{
    switch (cat)
    {
    case output::report_category::configuration:
        return BinaryValentineContext::ReportCategory::ReportCategoryConfiguration;
    case output::report_category::format:
        return BinaryValentineContext::ReportCategory::ReportCategoryFormat;
    case output::report_category::optimization:
        return BinaryValentineContext::ReportCategory::ReportCategoryOptimization;
    case output::report_category::security:
        return BinaryValentineContext::ReportCategory::ReportCategorySecurity;
    case output::report_category::system:
        return BinaryValentineContext::ReportCategory::ReportCategorySystem;
    default:
        return BinaryValentineContext::ReportCategory::ReportCategorySystem;
    }
}

BinaryValentineContext::ReportLevel BinaryValentineContext::toReportLevel(
    output::report_level level)
{
    switch (level)
    {
    case output::report_level::critical:
        return BinaryValentineContext::ReportLevelCritical;
    case output::report_level::error:
        return BinaryValentineContext::ReportLevelError;
    case output::report_level::info:
        return BinaryValentineContext::ReportLevelInfo;
    case output::report_level::warning:
        return BinaryValentineContext::ReportLevelWarning;
    default:
        return BinaryValentineContext::ReportLevelCritical;
    }
}

const QRegularExpression& BinaryValentineContext::getDescriptionArgRegex()
{
    static const QRegularExpression argRegex(R"(\{([a-z0-9_]+)(:[^\}]+?)?\}([^\}]|$))");
    return argRegex;
}

const QVariantMap& BinaryValentineContext::getAllAvailableReports()
{
    static const auto reports = getAvailableReportsImpl(
       false, ImmutableContext::getImmutableContext().get_rules().get_reports(),
        ImmutableContext::getImmutableContext().get_combined_rules().get_reports());
    return reports;
}

const QVariantMap& BinaryValentineContext::getAllAvailableReportsWithDescriptionArgs()
{
    static const auto reports = getAvailableReportsImpl(
        true, ImmutableContext::getImmutableContext().get_rules().get_reports(),
        ImmutableContext::getImmutableContext().get_combined_rules().get_reports());
    return reports;
}

} // namespace bv
