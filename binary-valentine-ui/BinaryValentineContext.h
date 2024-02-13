#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QVariantMap>

namespace bv
{

namespace output
{
enum class report_category;
enum class report_level;
} //namespace output

class BinaryValentineContext : public QObject
{
public:
    enum ReportCategory
    {
        ReportCategorySystem,
        ReportCategoryOptimization,
        ReportCategorySecurity,
        ReportCategoryConfiguration,
        ReportCategoryFormat
    };

    enum ReportLevel
    {
        ReportLevelInfo,
        ReportLevelWarning,
        ReportLevelError,
        ReportLevelCritical
    };

    Q_ENUM(ReportCategory);
    Q_ENUM(ReportLevel);

private:
    Q_OBJECT
    Q_PROPERTY(QVariantMap allAvailableReports READ getAllAvailableReports FINAL CONSTANT)
    Q_PROPERTY(QVariantMap allAvailableReportsWithDescriptionArgs
                   READ getAllAvailableReportsWithDescriptionArgs FINAL CONSTANT)
    QML_ELEMENT
    QML_SINGLETON

public:
    [[nodiscard]]
    const QVariantMap& getAllAvailableReports();
    [[nodiscard]]
    const QVariantMap& getAllAvailableReportsWithDescriptionArgs();
    [[nodiscard]]
    static const QRegularExpression& getDescriptionArgRegex();

    [[nodiscard]]
    static ReportCategory toReportCategory(output::report_category cat);
    [[nodiscard]]
    static ReportLevel toReportLevel(output::report_level level);

    [[nodiscard]]
    static BinaryValentineContext& getInstance();
};

} // namespace bv
