#include "AnalyzedFileTracker.h"

#include <bit>
#include <ranges>
#include <shared_mutex>
#include <utility>
#include <vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ranked_index.hpp>

#include <QString>

#include "BinaryValentineContext.h"
#include "InMemoryReportOutput.h"

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/string/rule_report_resource_helper.h"

namespace bv
{

struct EntitiesContainerValue
{
    QString path;
    int fileIndex{};
};

struct AnalyzedFileTracker::Impl
{
    using EntitiesContainer = boost::multi_index_container<
        EntitiesContainerValue,
        boost::multi_index::indexed_by<
            boost::multi_index::ranked_unique<
                    boost::multi_index::member<
                        EntitiesContainerValue, QString, &EntitiesContainerValue::path>
                >
            >
        >;

    EntitiesContainer fullFiles;
    std::vector<std::shared_ptr<const InMemoryReportOutput>> commonReports;
    std::vector<std::shared_ptr<const InMemoryReportOutput>> fileReports;

    std::shared_mutex mutex;
    std::vector<std::shared_ptr<const InMemoryReportOutput>> newlyAddedFiles;
};

AnalyzedFileTracker::AnalyzedFileTracker()
    : impl_(std::make_unique<Impl>())
{
    // Common report
    impl_->fullFiles.emplace(QString{});
}

AnalyzedFileTracker::~AnalyzedFileTracker() = default;

void AnalyzedFileTracker::registerFile(
    const std::shared_ptr<const InMemoryReportOutput>& file)
{
    std::unique_lock lock(impl_->mutex);
    impl_->newlyAddedFiles.emplace_back(file);
}

namespace
{
InMemoryReportOutput::PerLevelCategoriesType getPerLevelCategories(
    const std::shared_ptr<const InMemoryReportOutput>& report)
{
    auto lock = report->lockShared();
    return report->getPerLevelCategories();
}
} //namespace

QVariantList AnalyzedFileTracker::getAllFiles(RuleSelectorNode::ReportCategories categories,
                                              RuleSelectorNode::ReportLevels levels)
{
    getNewlyAddedFiles();

    QVariantList result;
    for (const auto& file : impl_->fullFiles)
    {
        if (!file.fileIndex) {
            //Common report
            continue;
        }

        const auto levelCategories = getPerLevelCategories(
            impl_->fileReports[file.fileIndex - 1]);

        std::size_t presentLevels = false;
        for (std::size_t i = 0; i != static_cast<std::size_t>(output::report_level::max); ++i)
        {
            if ((levels & (1u << i)) && (levelCategories[i] & categories))
                presentLevels |= (1u << i);
        }

        if (presentLevels)
        {
            result.append(QVariantMap{
                {"path", file.path},
                {"fileIndex", file.fileIndex},
                {"level", std::bit_width(presentLevels)}
            });
        }
    }

    return result;
}

QVariantList AnalyzedFileTracker::getNewlyAddedFiles()
{
    std::vector<std::shared_ptr<const InMemoryReportOutput>> newFiles;

    {
        std::shared_lock lock(impl_->mutex);
        newFiles = std::move(impl_->newlyAddedFiles);
        impl_->newlyAddedFiles.clear();
    }

    QVariantList result;
    result.reserve(newFiles.size());

    for (auto& report : newFiles)
    {
        if (!report->get_entity()) {
            impl_->commonReports.emplace_back(std::move(report));
            continue;
        }

        const auto utf8Path = report->get_entity()->get_utf8_path();
        auto utf8PathQstr = QString::fromUtf8(
            utf8Path.data(), utf8Path.size());

        const int fileIndex = static_cast<int>(impl_->fileReports.size()) + 1;
        auto it = impl_->fullFiles.emplace(EntitiesContainerValue{
                                                                  .path = utf8PathQstr,
                                                                  .fileIndex = fileIndex}).first;

        impl_->fileReports.emplace_back(std::move(report));

        result.append(QVariantMap{
            {"path", std::move(utf8PathQstr)},
            {"index", impl_->fullFiles.rank(it)},
            {"fileIndex", fileIndex},
            {"level", 0u}
        });
    }

    return result;
}

QVariantList AnalyzedFileTracker::produceReport(
    const std::span<const std::shared_ptr<const InMemoryReportOutput>> reportOutputs,
    RuleSelectorNode::ReportCategories categories,
    RuleSelectorNode::ReportLevels levels)
{
    QVariantList result;
    output::saved_rule_reports_type savedRuleReports;
    output::saved_common_reports_type savedCommonReports;

    for (const auto& reportOutput : reportOutputs)
    {
        const auto lock = reportOutput->lockShared();
        savedRuleReports.insert(savedRuleReports.end(),
                                reportOutput->get_rule_reports().begin(),
                                reportOutput->get_rule_reports().end());
        savedCommonReports.insert(savedCommonReports.end(),
                                  reportOutput->get_common_reports().begin(),
                                  reportOutput->get_common_reports().end());
    }

    std::erase_if(savedCommonReports, [&levels](
                                          const output::saved_common_report& report) {
        return !levels.testFlag(static_cast<RuleSelectorNode::ReportLevel>(
            1 << static_cast<int>(report.level)));
    });
    std::erase_if(savedRuleReports, [&levels, &categories](
                                        const output::saved_rule_report& report) {
        return !levels.testFlag(static_cast<RuleSelectorNode::ReportLevel>(
                   1 << static_cast<int>(report.report.get_report_level())))
               || !categories.testFlag(static_cast<RuleSelectorNode::ReportCategory>(
                   1 << static_cast<int>(report.report.get_report_category())));
    });

    result.reserve(savedRuleReports.size() + savedCommonReports.size());

    std::sort(savedCommonReports.begin(), savedCommonReports.end(),
              [](const output::saved_common_report& l, const output::saved_common_report& r)
              {
                  return l.level > r.level;
              });

    for (const auto& report : savedCommonReports)
    {
        result.append(QVariantMap{
            {"id", QString{}},
            {"level", BinaryValentineContext::toReportLevel(report.level)},
            {"message", QString::fromStdString(report.formatted_message)},
            {"category", 0},
            {"name", QString{}}
        });
    }

    std::sort(savedRuleReports.begin(), savedRuleReports.end(),
              [](const output::saved_rule_report& l, const output::saved_rule_report& r)
              {
                  if (l.report.get_report_level() == r.report.get_report_level())
                      return l.report.get_string_uid() < r.report.get_string_uid();

                  return l.report.get_report_level() > r.report.get_report_level();
              });

    for (const auto& report : savedRuleReports)
    {
        string::rule_report_resource_helper helper(
            report.report, reportOutputs[0]->get_resource_provider());

        const auto stringUid = report.report.get_string_uid();
        const auto& name = helper.get_report_uid_name();

        result.append(QVariantMap{
            {"id", QLatin1StringView(stringUid)},
            {"category", BinaryValentineContext::toReportCategory(report.report.get_report_category())},
            {"level", BinaryValentineContext::toReportLevel(report.report.get_report_level())},
            {"name", QLatin1StringView(name)},
            {"message", QString::fromStdString(report.formatted_message)}
        });
    }

    return result;
}

QVariantList AnalyzedFileTracker::getReports(int fileIndex,
                                             RuleSelectorNode::ReportCategories categories,
                                             RuleSelectorNode::ReportLevels levels)
{
    if (fileIndex == 0)
        return produceReport(impl_->commonReports, categories, levels);

    if (fileIndex < 0 || fileIndex > impl_->fileReports.size())
        return {};

    return produceReport(std::ranges::single_view(impl_->fileReports[fileIndex - 1]), categories, levels);
}

} // namespace bv
