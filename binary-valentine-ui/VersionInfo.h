#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QString>
#include <QtQmlIntegration>

namespace bv
{

class VersionInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString toolName READ getToolName FINAL CONSTANT)
    Q_PROPERTY(QString fullToolName READ getToolFullName FINAL CONSTANT)
    Q_PROPERTY(QString docsWebsiteUrl READ getDocsWebsiteUrl FINAL CONSTANT)
    Q_PROPERTY(QString websiteUrl READ getWebsiteUrl FINAL CONSTANT)
    Q_PROPERTY(QString githubUrl READ getGithubUrl FINAL CONSTANT)
    Q_PROPERTY(QString qtVersion READ getQtVersion FINAL CONSTANT)
    Q_PROPERTY(QString appVersion READ getVersion FINAL CONSTANT)
    Q_PROPERTY(QString authorName READ getAuthorName FINAL CONSTANT)
    QML_ELEMENT
    QML_SINGLETON

public:
    VersionInfo(QObject* parent = nullptr);

    [[nodiscard]]
    QString getToolFullName() const;
    [[nodiscard]]
    QString getToolName() const;
    [[nodiscard]]
    QString getDocsWebsiteUrl() const;
    [[nodiscard]]
    QString getWebsiteUrl() const;
    [[nodiscard]]
    QString getGithubUrl() const;
    [[nodiscard]]
    QString getQtVersion() const;
    [[nodiscard]]
    QString getVersion() const;
    [[nodiscard]]
    QString getAuthorName() const;
    [[nodiscard]]
    Q_INVOKABLE QString getRuleInfoUrl(QString ruleUid) const;
};

} //namespace bv
