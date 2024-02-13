#pragma once

#include <QLatin1StringView>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"

namespace bv
{

class ScanTargetNode final : public ProjectTreeNode
{
public:
    static constexpr QLatin1StringView pathFilterTypeKey{"pathFilterType"};
    static constexpr QLatin1StringView pathFilterRegexKey{"regex"};

    enum class PathType
    {
        Absolute,
        Relative
    };

    enum PathFilterType
    {
        IncludePathWhenMatches,
        ExcludePathWhenMatches
    };
    Q_ENUM(PathFilterType);

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(ScanTarget);
    QML_UNCREATABLE("Cannot be created from QML");

public:
    Q_PROPERTY(QString path MEMBER path_ NOTIFY pathChanged);
    Q_PROPERTY(bool isRecursive READ isRecursive
                   WRITE setRecursive NOTIFY isRecursiveChanged);

signals:
    void isRecursiveChanged();
    void switchPathType(ScanTargetNode* target, PathType type);
    void pathChanged();
    void ruleSelectorRequested(ScanTargetNode* target);

public:
    explicit ScanTargetNode(ProjectTreeNode* parent, QString path);

public:
    Q_INVOKABLE void deleteTarget();
    Q_INVOKABLE void makeRelative();
    Q_INVOKABLE void makeAbsolute();
    Q_INVOKABLE bool setPath(QString path);

    [[nodiscard]]
    Q_INVOKABLE QVariantMap addNewPathFilter();
    QVariantMap addPathFilter(PathFilterType type, QString regex);

    Q_INVOKABLE void deletePathFilter(int index);
    Q_INVOKABLE void setPathFilterType(int index, PathFilterType type);
    Q_INVOKABLE bool setPathFilterRegex(int index, QString regex);

    Q_INVOKABLE void addRuleSelector();

    void setRecursive(bool recursive);

public:
    [[nodiscard]]
    Q_INVOKABLE QVariantList getPathFilters() const;

    [[nodiscard]]
    const QString& getPath() const noexcept
    {
        return path_;
    }

    [[nodiscard]]
    bool isRecursive() const noexcept
    {
        return recursive_;
    }

private:
    virtual QVariant dataImpl(int role) const override;

private:
    QString path_;
    bool recursive_{true};
    QVariantList pathFilters_;
};
} // namespace bv
