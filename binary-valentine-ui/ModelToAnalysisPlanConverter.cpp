#include "ModelToAnalysisPlanConverter.h"

#include <cstddef>
#include <cstdint>
#include <limits>

#include "ImmutableContext.h"
#include "ProjectTreeNodes.h"
#include "ProjectSettingsNode.h"
#include "QtStdTypeConverter.h"
#include "ReportFilterNode.h"
#include "ReportOutputsNode.h"
#include "RuleSelectorNode.h"
#include "ScanTargetNode.h"
#include "TargetsNode.h"

#include "binary_valentine/analysis/analysis_plan_parse_helpers.h"
#include "binary_valentine/core/user_error.h"

namespace bv
{

namespace
{

void convertProjectSettings(const ProjectSettingsNode& projectSettings,
                            analysis::analysis_plan& result)
{
    result.enable_combined_analysis(projectSettings.getCombinedAnalysis());
    if (projectSettings.hasCustomThreadCount()
        && projectSettings.getThreadCount() > 0
        && projectSettings.getThreadCount() < (std::numeric_limits<std::uint8_t>::max)())
    {
        result.set_thread_count(projectSettings.getThreadCount());
    }

    if (!projectSettings.getRootPath().isEmpty())
    {
        result.set_root_path(
            QtStdTypeConverter::toPath(projectSettings.getRootPath(),
                                       core::user_errc::invalid_root_path));
    }

    switch (projectSettings.getAnalysisLimitType())
    {
    case ProjectSettingsNode::AnalysisLimitTypeSize:
        if (projectSettings.getTotalFileSizeLimitMb() > 0
            && projectSettings.getTotalFileSizeLimitMb()
                   < (std::numeric_limits<std::uint64_t>::max)() / (1024 * 1024))
        {
            result.set_preload_limit(analysis::max_loaded_targets_size{
                .value = static_cast<std::uint64_t>(
                             projectSettings.getTotalFileSizeLimitMb()) * 1024 * 1024
            });
        }
        break;

    case ProjectSettingsNode::AnalysisLimitTypeFileCount:
        if (projectSettings.getConcurrentFilesLimit() > 0
            && projectSettings.getConcurrentFilesLimit()
                   < (std::numeric_limits<std::uint8_t>::max)())
        {
            result.set_preload_limit(analysis::max_concurrent_tasks{
                .value = static_cast<std::uint8_t>(
                    projectSettings.getConcurrentFilesLimit())
            });
        }
        break;

    default:
        break;
    }
}

void convertSelector(const RuleSelectorNode& ruleSelector, core::rule_selector& selector)
{
    const auto& immutableContext = ImmutableContext::getImmutableContext();
    if (ruleSelector.getReportSelectionPolicy() != RuleSelectorNode::SelectAllReports)
    {
        if (ruleSelector.getReportSelectionPolicy() == RuleSelectorNode::IncludeReports)
            selector.set_mode(core::rule_selection_mode::include_selected);
        else
            selector.set_mode(core::rule_selection_mode::exclude_selected);

        const QVariantMap& selectedReports = ruleSelector.getSelectedReports();
        for (auto it = selectedReports.keyBegin(); it != selectedReports.keyEnd(); ++it)
        {
            const auto& key = *it;
            analysis::analysis_plan_parse_helpers::add_selected_report(
                key.toStdString(),
                immutableContext.get_rules(),
                immutableContext.get_combined_rules(),
                selector);
        }
    }

    if (!(ruleSelector.getReportLevels() & RuleSelectorNode::ReportLevelInfo))
        selector.exclude_report_level(output::report_level::info);
    if (!(ruleSelector.getReportLevels() & RuleSelectorNode::ReportLevelWarning))
        selector.exclude_report_level(output::report_level::warning);
    if (!(ruleSelector.getReportLevels() & RuleSelectorNode::ReportLevelError))
        selector.exclude_report_level(output::report_level::error);
    if (!(ruleSelector.getReportLevels() & RuleSelectorNode::ReportLevelCritical))
        selector.exclude_report_level(output::report_level::critical);

    if (!(ruleSelector.getReportCategories() & RuleSelectorNode::ReportCategoryConfiguration))
        selector.exclude_report_category(output::report_category::configuration);
    if (!(ruleSelector.getReportCategories() & RuleSelectorNode::ReportCategoryFormat))
        selector.exclude_report_category(output::report_category::format);
    if (!(ruleSelector.getReportCategories() & RuleSelectorNode::ReportCategoryOptimization))
        selector.exclude_report_category(output::report_category::optimization);
    if (!(ruleSelector.getReportCategories() & RuleSelectorNode::ReportCategorySecurity))
        selector.exclude_report_category(output::report_category::security);
    if (!(ruleSelector.getReportCategories() & RuleSelectorNode::ReportCategorySystem))
        selector.exclude_report_category(output::report_category::system);

    for (std::size_t i = 0, childCount = ruleSelector.childCount(); i != childCount; ++i)
    {
        const auto& reportFilter = *static_cast<const ReportFilterNode*>(
            ruleSelector.childAt(i));

        auto [filterRef, added] = selector.get_or_create_report_selector(
            analysis::analysis_plan_parse_helpers::get_report_uid(
                reportFilter.getReportUid().toStdString(),
                immutableContext.get_rules(),
                immutableContext.get_combined_rules()));

        auto& filter = filterRef.get();
        if (reportFilter.getAggregationMode() == ReportFilterNode::ReportFilterAggregationModeAllMatch)
            filter.set_aggregation_mode(core::aggregation_mode::all);
        else
            filter.set_aggregation_mode(core::aggregation_mode::any);

        if (reportFilter.getFilterMode() == ReportFilterNode::ReportFilterModeDisableWhenMatches)
            filter.set_selection_mode(core::rule_selection_mode::exclude_selected);
        else
            filter.set_selection_mode(core::rule_selection_mode::include_selected);

        for (auto it = reportFilter.getRegexes().constKeyValueBegin(),
             end = reportFilter.getRegexes().constKeyValueEnd();
             it != end; ++it)
        {
            const auto& [arg, regex] = *it;
            filter.add_regex(arg.toStdString(), regex.toStdString());
        }
    }
}

void convertTargets(const TargetsNode& targets, analysis::analysis_plan& result)
{
    const std::size_t targetsCount = targets.childCount();
    result.reserve_targets(targetsCount);
    for (std::size_t i = 0; i != targetsCount; ++i)
    {
        const auto& targetNode = *static_cast<const ScanTargetNode*>(targets.childAt(i));

        auto& target = result.emplace_target(QtStdTypeConverter::toPath(
            targetNode.getPath(), core::user_errc::invalid_target_path));
        target.set_recursive(targetNode.isRecursive());

        const QVariantList pathFilters = targetNode.getPathFilters();
        auto& targetFilter = target.get_target_filter();
        for (const auto& filter : pathFilters)
        {
            const auto map = filter.toMap();
            const auto filterType = static_cast<ScanTargetNode::PathFilterType>(
                map[ScanTargetNode::pathFilterTypeKey].toInt());
            const auto regex = map[ScanTargetNode::pathFilterRegexKey].toString().toStdString();
            if (filterType == ScanTargetNode::IncludePathWhenMatches)
                targetFilter.add_include_regex(regex);
            else
                targetFilter.add_exclude_regex(regex);
        }

        if (targetNode.childCount() == 1u)
        {
            convertSelector(
                *static_cast<const RuleSelectorNode*>(targetNode.childAt(0u)),
                target.emplace_rule_selector());
        }
    }
}

void convertOutputReports(const ReportOutputsNode& outputs, analysis::analysis_plan& result,
                          bool skipTerminalOutput)
{
    if (outputs.useTerminalOutput() && !skipTerminalOutput)
        result.emplace_output_report(analysis::result_report_terminal{});

    const QVariantList outputFiles = outputs.getOutputFiles();
    for (const auto& outputFile : outputFiles)
    {
        const auto map = outputFile.toMap();
        analysis::result_report_file_type fileType{};
        if (map[ReportOutputsNode::fileFormatKey].toInt() == ReportOutputsNode::FileFormatText)
            fileType = analysis::result_report_file_type::text;
        else
            fileType = analysis::result_report_file_type::sarif;

        result.emplace_output_report(analysis::result_report_file(
            QtStdTypeConverter::toPath(
                map[ReportOutputsNode::filePathKey].toString(),
                core::user_errc::invalid_report_path),
            fileType));
    }
}

} //namespace

analysis::analysis_plan ModelToAnalysisPlanConverter::toPlan(
    const ProjectTreeNodes& nodes,
    const ModelToAnalysisPlanConverterOpts& opts)
{
    analysis::analysis_plan result;
    result.set_language(analysis::default_language);
    convertProjectSettings(nodes.projectSettingsNode, result);
    convertSelector(nodes.ruleSelectorNode, result.get_global_rule_selector());
    convertTargets(nodes.targetsNode, result);
    convertOutputReports(nodes.reportOutputsNode, result, opts.skipTerminalOutput);
    return result;
}

} // namespace bv
