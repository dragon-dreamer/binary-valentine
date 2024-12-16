#include "AnalysisPlanModelPopulator.h"

#include <algorithm>
#include <string_view>
#include <utility>
#include <variant>

#include <QVariantMap>

#include "ProjectModel.h"
#include "ProjectSettingsNode.h"
#include "ReportFilterNode.h"
#include "ReportOutputsNode.h"
#include "RuleSelectorNode.h"
#include "ScanTargetNode.h"
#include "TargetsNode.h"
#include "QtStdTypeConverter.h"

#include "binary_valentine/analysis/result_report_file.h"
#include "utilities/variant_helpers.h"

namespace bv
{

namespace
{

void populateProjectSettings(const analysis::analysis_plan& plan,
                             ProjectSettingsNode& projectSettings)
{
    if (!plan.get_root_path().empty())
    {
        projectSettings.setRootPath(
            QtStdTypeConverter::toString(plan.get_root_path()));
    }

    projectSettings.setCombinedAnalysis(plan.do_combined_analysis());

    if (plan.get_thread_count())
    {
        projectSettings.setUseCustomThreadCount(true);
        projectSettings.setThreadCount(std::clamp<int>(plan.get_thread_count(), 1, 99));
    }

    std::visit(utilities::overloaded{
                   [&projectSettings](const analysis::max_concurrent_tasks& limit) {
                       projectSettings.setAnalysisLimitType(
                           ProjectSettingsNode::AnalysisLimitTypeFileCount);
                       projectSettings.setConcurrentFilesLimit(std::clamp<int>(limit.value, 1, 100));
                   },
                   [&projectSettings](const analysis::max_loaded_targets_size& limit) {
                       if (limit.value == analysis::max_loaded_targets_size{}.value) {
                           projectSettings.setAnalysisLimitType(
                               ProjectSettingsNode::AnalysisLimitTypeDefault);
                           return;
                       }

                       projectSettings.setAnalysisLimitType(
                           ProjectSettingsNode::AnalysisLimitTypeSize);
                       projectSettings.setTotalFileSizeLimitMb(
                           std::clamp<int>(limit.value / (1024 * 1024), 32, 65536));
                   }
               }, plan.get_preload_limit());
}

void populateSelector(const core::rule_selector& selector,
                      RuleSelectorNode& ruleSelector)
{
    using enum RuleSelectorNode::ReportLevel;
    using enum RuleSelectorNode::ReportCategory;

    RuleSelectorNode::ReportLevels reportLevels{};
    RuleSelectorNode::ReportCategories reportCategories{};

    if (selector.is_enabled(output::report_category::configuration))
        reportCategories.setFlag(ReportCategoryConfiguration);
    if (selector.is_enabled(output::report_category::format))
        reportCategories.setFlag(ReportCategoryFormat);
    if (selector.is_enabled(output::report_category::optimization))
        reportCategories.setFlag(ReportCategoryOptimization);
    if (selector.is_enabled(output::report_category::security))
        reportCategories.setFlag(ReportCategorySecurity);
    if (selector.is_enabled(output::report_category::system))
        reportCategories.setFlag(ReportCategorySystem);

    if (selector.is_enabled(output::report_level::info))
        reportLevels.setFlag(ReportLevelInfo);
    if (selector.is_enabled(output::report_level::warning))
        reportLevels.setFlag(ReportLevelWarning);
    if (selector.is_enabled(output::report_level::error))
        reportLevels.setFlag(ReportLevelError);
    if (selector.is_enabled(output::report_level::critical))
        reportLevels.setFlag(ReportLevelCritical);

    ruleSelector.setSelectedLevels(reportLevels);
    ruleSelector.setSelectedCategories(reportCategories);

    if (selector.get_selected_report_uids().empty())
        ruleSelector.setSelectionPolicy(RuleSelectorNode::ReportSelectionPolicy::SelectAllReports);
    else if (selector.get_rule_selection_mode() == core::rule_selection_mode::exclude_selected)
        ruleSelector.setSelectionPolicy(RuleSelectorNode::ReportSelectionPolicy::ExcludeReports);
    else
        ruleSelector.setSelectionPolicy(RuleSelectorNode::ReportSelectionPolicy::IncludeReports);

    QVariantMap selectedReports;
    for (const auto& uid : selector.get_selected_report_uids())
    {
        const std::string_view stringUid = uid.get_metadata()->get_string_uid();
        selectedReports[QLatin1StringView(stringUid)] = true;
    }
    ruleSelector.setSelectedReports(std::move(selectedReports));

    std::vector<std::pair<std::string_view,
                          std::reference_wrapper<const core::report_selector>>> sortedUids;
    sortedUids.reserve(selector.get_report_selectors().size());
    for (const auto& [uid, selector] : selector.get_report_selectors())
        sortedUids.emplace_back(uid.get_metadata()->get_string_uid(), selector);

    std::ranges::sort(sortedUids, [](const auto& l, const auto& r) {
        return l.first < r.first;
    });

    for (const auto& [uid, selector] : sortedUids)
    {
        ReportFilterNode& filterNode = *ruleSelector.addReportInOrder(
            QLatin1StringView(uid));
        filterNode.setAggregationMode(
            selector.get().get_aggregation_mode() == core::aggregation_mode::all
                ? ReportFilterNode::ReportFilterAggregationModeAllMatch
                : ReportFilterNode::ReportFilterAggregationModeAnyMatches);
        filterNode.setFilterMode(
            selector.get().get_selection_mode() == core::rule_selection_mode::exclude_selected
                ? ReportFilterNode::ReportFilterModeDisableWhenMatches
                : ReportFilterNode::ReportFilterModeEnableWhenMatches);

        for (const auto& [arg, regex] : selector.get().get_arg_regex_map())
            filterNode.setRegex(QLatin1StringView(arg), QString::fromStdString(regex.second));
    }
}

void populateOutputReports(const analysis::analysis_plan& plan, ReportOutputsNode& outputs)
{
    bool toTerminal = false;
    for (const auto& report : plan.get_result_reports())
    {
        std::visit(utilities::overloaded{
                       [](const analysis::result_report_in_memory&) {},
                       [&toTerminal](const analysis::result_report_terminal&) { toTerminal = true; },
                       [&outputs](const analysis::result_report_file& file) {
                           ReportOutputsNode::FileFormat reportFormat{};
                           QString reportTemplatePath;
                           switch (file.get_type())
                           {
                           case analysis::result_report_file_type::sarif:
                               reportFormat = ReportOutputsNode::FileFormatSarif;
                               break;
                           case analysis::result_report_file_type::text:
                               reportFormat = ReportOutputsNode::FileFormatText;
                               break;
                           case analysis::result_report_file_type::html_report:
                               reportFormat = ReportOutputsNode::FileFormatHtml;
                               if (auto it = file.get_extra_options().find(
                                       analysis::result_report_file::report_template_extra_argument_name);
                                   it != file.get_extra_options().cend()) {
                                   reportTemplatePath = QtStdTypeConverter::toString(
                                       it->second);
                               }
                               break;
                           }

                           outputs.addOutputFile(
                               reportFormat,
                               QtStdTypeConverter::toString(file.get_path()),
                               std::move(reportTemplatePath));
                       }
                   }, report);
    }

    outputs.enableTerminalOutput(toTerminal);
}

void populateTargets(const analysis::analysis_plan& plan, TargetsNode& targetsNode)
{
    const auto& targets = plan.get_targets();
    std::vector<std::size_t> sortedIndexes(plan.get_targets().size());
    std::iota(sortedIndexes.begin(), sortedIndexes.end(), 0);
    std::ranges::sort(sortedIndexes, [&targets](auto l, auto r) {
        return targets[l].get_path() < targets[r].get_path();
    });
    auto [from, to] = std::ranges::unique(
        sortedIndexes, [&targets](auto l, auto r) {
            return targets[l].get_path() == targets[r].get_path();
        });
    sortedIndexes.erase(from, to);

    std::vector<QString> paths;
    paths.reserve(sortedIndexes.size());
    for (const std::size_t index : sortedIndexes)
    {
        const auto& target = targets[index];
        paths.emplace_back(QtStdTypeConverter::toString(target.get_path()));
    }
    targetsNode.appendTargetsInOrder(paths);

    for (const std::size_t index : sortedIndexes)
    {
        const auto& target = targets[index];
        ScanTargetNode& targetNode = *static_cast<ScanTargetNode*>(
            targetsNode.childAt(index));

        targetNode.setRecursive(target.is_recursive());

        if (target.get_rule_selector())
        {
            targetNode.addRuleSelector();
            populateSelector(*target.get_rule_selector(),
                             *static_cast<RuleSelectorNode*>(targetNode.childAt(0)));
        }

        for (const auto& regex : target.get_target_filter().get_include_regex_strings())
        {
            targetNode.addPathFilter(ScanTargetNode::IncludePathWhenMatches,
                                     QString::fromStdString(regex));
        }

        for (const auto& regex : target.get_target_filter().get_exclude_regex_strings())
        {
            targetNode.addPathFilter(ScanTargetNode::ExcludePathWhenMatches,
                                     QString::fromStdString(regex));
        }
    }
}

} //namespace

void AnalysisPlanModelPopulator::populateModel(
    const analysis::analysis_plan& plan, ProjectModel& model)
{
    populateProjectSettings(plan, model.getProjectSettings());
    populateSelector(plan.get_global_rule_selector(), model.getGlobalSelector());
    populateTargets(plan, model.getTargets());
    populateOutputReports(plan, model.getReportOutputs());
}

} // namespace bv
