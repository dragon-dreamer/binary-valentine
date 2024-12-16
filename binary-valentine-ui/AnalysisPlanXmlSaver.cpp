#include "AnalysisPlanXmlSaver.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <variant>

#include <QFile>
#include <QXmlStreamWriter>

#include "binary_valentine/core/user_error.h"
#include "binary_valentine/version.h"
#include "utilities/variant_helpers.h"

#include "QtStdTypeConverter.h"
#include "VersionInfo.h"

namespace bv
{

namespace
{
QString getLoadedTargetsSize(std::uint64_t size)
{
    static constexpr std::array<char, 5> suffixes{'B', 'K', 'M', 'G', '\0'};
    auto suffix = suffixes.cbegin();
    while ((size / 1024) * 1024 == size && *(suffix + 1))
    {
        size /= 1024;
        ++suffix;
    }

    return QString::number(size) + *suffix;
}

void writePlanAttributes(const analysis::analysis_plan& plan, QXmlStreamWriter& stream)
{
    if (const auto& path = plan.get_root_path(); !path.empty())
        stream.writeAttribute("root_path", QtStdTypeConverter::toString(path));

    if (const auto count = plan.get_thread_count(); count != 0)
        stream.writeAttribute("thread_count", QString::number(count));

    if (plan.do_combined_analysis())
        stream.writeAttribute("combined_analysis", "true");

    std::visit(utilities::overloaded{
                   [&stream] (const analysis::max_concurrent_tasks& tasks) {
                       stream.writeAttribute("max_concurrent_tasks",
                                             QString::number(tasks.value));
                   },
                   [&stream] (const analysis::max_loaded_targets_size& size) {
                       stream.writeAttribute("max_loaded_targets_size",
                                             getLoadedTargetsSize(size.value));
                   }
               }, plan.get_preload_limit());
}

bool allLevelsEnabled(const core::rule_selector& selector)
{
    for (std::size_t i = 0; i != static_cast<std::size_t>(output::report_level::max); ++i)
    {
        if (!selector.is_enabled(static_cast<output::report_level>(i)))
            return false;
    }
    return true;
}

bool allCategoriesEnabled(const core::rule_selector& selector)
{
    for (std::size_t i = 0; i != static_cast<std::size_t>(output::report_category::max); ++i)
    {
        if (!selector.is_enabled(static_cast<output::report_category>(i)))
            return false;
    }
    return true;
}

std::string_view getLevelName(output::report_level level)
{
    switch (level)
    {
    case output::report_level::info: return "info";
    case output::report_level::warning: return "warning";
    case output::report_level::error: return "error";
    case output::report_level::critical: return "critical";
    default: break;
    }
    return "";
}

std::string_view getCategoryName(output::report_category category)
{
    switch (category)
    {
    case output::report_category::configuration: return "configuration";
    case output::report_category::format: return "format";
    case output::report_category::optimization: return "optimization";
    case output::report_category::security: return "security";
    case output::report_category::system: return "system";
    default: break;
    }
    return "";
}

void addExcludedLevels(const core::rule_selector& selector, QXmlStreamWriter& stream)
{
    stream.writeStartElement("exclude_levels");
    for (std::size_t i = 0; i != static_cast<std::size_t>(output::report_level::max); ++i)
    {
        const auto level = static_cast<output::report_level>(i);
        if (!selector.is_enabled(level))
            stream.writeTextElement("level", getLevelName(level));
    }
    stream.writeEndElement(); //exclude_levels
}

void addExcludedCategories(const core::rule_selector& selector, QXmlStreamWriter& stream)
{
    stream.writeStartElement("exclude_categories");
    for (std::size_t i = 0; i != static_cast<std::size_t>(output::report_category::max); ++i)
    {
        const auto category = static_cast<output::report_category>(i);
        if (!selector.is_enabled(category))
            stream.writeTextElement("category", getCategoryName(category));
    }
    stream.writeEndElement(); //exclude_categories
}

void addSelectedReports(core::rule_selection_mode mode,
                        const core::rule_selector::report_uids_set& uids,
                        QXmlStreamWriter& stream)
{
    stream.writeStartElement(mode == core::rule_selection_mode::exclude_selected
                                 ? "exclude_reports"
                                 : "include_reports");

    for (const auto& uid : uids)
        stream.writeTextElement("report", uid.get_metadata()->get_string_uid());

    stream.writeEndElement(); //exclude_reports/include_reports
}

void addReportSelectors(const core::rule_selector::report_selectors_map& selectors,
                        QXmlStreamWriter& stream)
{
    for (const auto& [uid, selector] : selectors)
    {
        stream.writeStartElement("report_filter");
        stream.writeAttribute("report", uid.get_metadata()->get_string_uid());
        stream.writeAttribute("selection_mode",
                              selector.get_selection_mode()
                                      == core::rule_selection_mode::exclude_selected
                                  ? "exclude" : "include");
        stream.writeAttribute("aggregation_mode",
                              selector.get_aggregation_mode()
                                      == core::aggregation_mode::all
                                  ? "all" : "any");

        for (const auto& [arg, regex] : selector.get_arg_regex_map())
        {
            stream.writeStartElement("regex");
            stream.writeAttribute("arg", arg);
            stream.writeCharacters(regex.second);
            stream.writeEndElement(); //regex
        }

        stream.writeEndElement(); //report_filter
    }
}

void writeSelector(const core::rule_selector& selector,
                   std::string_view nodeName, QXmlStreamWriter& stream,
                   bool writeIfEmpty)
{
    const bool hasExcludedLevels = !allLevelsEnabled(selector);
    const bool hasExcludedCategories = !allCategoriesEnabled(selector);
    if (!writeIfEmpty)
    {
        if (selector.get_report_selectors().empty()
            && selector.get_selected_report_uids().empty()
            && selector.get_rule_selection_mode() == core::rule_selection_mode::exclude_selected
            && !hasExcludedLevels
            && !hasExcludedCategories)
        {
            return;
        }
    }

    stream.writeStartElement(nodeName);

    if (hasExcludedLevels)
        addExcludedLevels(selector, stream);

    if (hasExcludedCategories)
        addExcludedCategories(selector, stream);

    if (!selector.get_selected_report_uids().empty()
        || selector.get_rule_selection_mode() != core::rule_selection_mode::exclude_selected)
    {
        addSelectedReports(selector.get_rule_selection_mode(),
                           selector.get_selected_report_uids(),
                           stream);
    }

    if (!selector.get_report_selectors().empty())
        addReportSelectors(selector.get_report_selectors(), stream);

    stream.writeEndElement(); //nodeName
}

void writeTargetFilter(const analysis::target_filter& filter, QXmlStreamWriter& stream)
{
    if (filter.get_exclude_regex_strings().empty()
        && filter.get_include_regex_strings().empty())
    {
        return;
    }

    stream.writeStartElement("filter");

    for (const auto& elem : filter.get_exclude_regex_strings())
        stream.writeTextElement("exclude", elem);

    for (const auto& elem : filter.get_include_regex_strings())
        stream.writeTextElement("include", elem);

    stream.writeEndElement(); //filter
}

void writeTargets(const std::vector<analysis::plan_target>& targets, QXmlStreamWriter& stream)
{
    if (targets.empty())
        return;

    stream.writeStartElement("targets");

    for (const auto& target : targets)
    {
        stream.writeStartElement("target");
        stream.writeAttribute("path", QtStdTypeConverter::toString(target.get_path()));
        if (!target.is_recursive())
            stream.writeAttribute("recursive", "false");

        if (target.get_rule_selector())
            writeSelector(*target.get_rule_selector(), "rule_selector", stream, true);

        writeTargetFilter(target.get_target_filter(), stream);

        stream.writeEndElement(); //target
    }

    stream.writeEndElement(); //targets
}

void writeReports(const std::vector<analysis::result_report_type>& reports, QXmlStreamWriter& stream)
{
    if (reports.empty())
        return;

    stream.writeStartElement("reports");

    for (const auto& report : reports)
    {
        std::visit(utilities::overloaded{
                       [&stream] (analysis::result_report_terminal) {
                           stream.writeEmptyElement("terminal");
                       },
                       [&stream] (const analysis::result_report_file& file) {
                           QAnyStringView elementName;
                           switch (file.get_type())
                           {
                           case analysis::result_report_file_type::text:
                               elementName = "text";
                               break;
                           case analysis::result_report_file_type::sarif:
                               elementName = "sarif";
                               break;
                           case analysis::result_report_file_type::html_report:
                               elementName = "html";
                               break;
                           }

                           if (file.get_extra_options().empty())
                           {
                               stream.writeTextElement(
                                   elementName,
                                   QtStdTypeConverter::toString(file.get_path()));
                           }
                           else
                           {
                               stream.writeStartElement(elementName);
                               for (const auto& [k, v] : file.get_extra_options())
                               {
                                   stream.writeAttribute(
                                       QtStdTypeConverter::toString(k),
                                       QtStdTypeConverter::toString(v));

                               }
                               stream.writeCharacters(
                                   QtStdTypeConverter::toString(file.get_path()));
                               stream.writeEndElement(); //elementName
                           }
                       },
                       [] (analysis::result_report_in_memory) {}
                   }, report);
    }


    stream.writeEndElement(); //reports
}
} // namespace

QString AnalysisPlanXmlSaver::toXml(const analysis::analysis_plan& plan)
{
    QString result;
    QXmlStreamWriter stream(&result);

    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeComment("Created with " + VersionInfo{}.getToolFullName()
                        + ", " + QLatin1StringView(version::tool_website));

    stream.writeStartElement("plan");

    writePlanAttributes(plan, stream);
    writeSelector(plan.get_global_rule_selector(), "global_selector", stream, false);
    writeTargets(plan.get_targets(), stream);
    writeReports(plan.get_result_reports(), stream);

    stream.writeEndElement(); //plan

    stream.writeEndDocument();

    return result;
}

void AnalysisPlanXmlSaver::saveToXml(const analysis::analysis_plan& plan, QString path)
{
    QString serializedXml = toXml(plan);

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        throw core::user_error(core::user_errc::unable_to_write_file,
                               core::user_error::arg_type("error",
                                                          file.errorString().toStdString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << serializedXml;
}

} // namespace bv
