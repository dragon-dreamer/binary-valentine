#pragma once

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"

namespace bv
{

class ReportFilterNode final : public ProjectTreeNode
{
public:
    enum ReportFilterMode
    {
        ReportFilterModeEnableWhenMatches,
        ReportFilterModeDisableWhenMatches
    };
    Q_ENUM(ReportFilterMode);

    enum ReportFilterAggregationMode
    {
        ReportFilterAggregationModeAllMatch,
        ReportFilterAggregationModeAnyMatches
    };
    Q_ENUM(ReportFilterAggregationMode);

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(ReportFilter);
    QML_UNCREATABLE("Cannot be created from QML");
    Q_PROPERTY(QString reportUid MEMBER reportUid_ FINAL CONSTANT);
    Q_PROPERTY(QStringList descriptionArgs MEMBER descriptionArgs_ FINAL CONSTANT);
    Q_PROPERTY(QVariantMap reportInfo MEMBER reportInfo_ FINAL CONSTANT);
    Q_PROPERTY(ReportFilterMode filterMode READ getFilterMode
                   WRITE setFilterMode NOTIFY filterModeChanged);
    Q_PROPERTY(ReportFilterAggregationMode filterAggregationMode READ getAggregationMode
                   WRITE setAggregationMode NOTIFY filterAggregationModeChanged);

public:
    explicit ReportFilterNode(ProjectTreeNode* parent, QString reportUid);

signals:
    void filterModeChanged();
    void filterAggregationModeChanged();

public:
    [[nodiscard]]
    const QString& getReportUid() const;

    [[nodiscard]]
    ReportFilterMode getFilterMode() const noexcept
    {
        return filterMode_;
    }

    [[nodiscard]]
    ReportFilterAggregationMode getAggregationMode() const noexcept
    {
        return filterAggregationMode_;
    }

    [[nodiscard]]
    const QMap<QString, QString>& getRegexes() const noexcept
    {
        return regexes_;
    }

public:
    void setFilterMode(ReportFilterMode mode);
    void setAggregationMode(ReportFilterAggregationMode mode);

public:
    Q_INVOKABLE void deleteFilter();
    Q_INVOKABLE bool setRegex(QString argName, QString regex);
    [[nodiscard]]
    Q_INVOKABLE QString getRegex(QString argName) const;

private:
    virtual QVariant dataImpl(int role) const override;

private:
    QString reportUid_;
    QVariantMap reportInfo_;
    QStringList descriptionArgs_;
    QMap<QString, QString> regexes_;
    ReportFilterMode filterMode_{ReportFilterModeEnableWhenMatches};
    ReportFilterAggregationMode filterAggregationMode_{ReportFilterAggregationModeAllMatch};
};

} // namespace bv

