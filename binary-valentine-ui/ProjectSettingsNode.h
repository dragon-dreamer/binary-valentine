#pragma once

#include <QObject>
#include <QString>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"

namespace bv
{

class ProjectSettingsNode final : public ProjectTreeNode
{
public:
    enum AnalysisLimitType
    {
        AnalysisLimitTypeDefault,
        AnalysisLimitTypeSize,
        AnalysisLimitTypeFileCount
    };
    Q_ENUM(AnalysisLimitType);

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(ProjectSettings);
    QML_UNCREATABLE("Cannot be created from QML");
    Q_PROPERTY(int threadCount READ getThreadCount WRITE setThreadCount NOTIFY threadCountChanged);
    Q_PROPERTY(bool combinedAnalysis READ getCombinedAnalysis
                   WRITE setCombinedAnalysis NOTIFY combinedAnalysisChanged);
    Q_PROPERTY(bool useCustomThreadCount READ hasCustomThreadCount
                   WRITE setUseCustomThreadCount NOTIFY useCustomThreadCountChanged);
    Q_PROPERTY(AnalysisLimitType analysisLimitType READ getAnalysisLimitType
                   WRITE setAnalysisLimitType NOTIFY analysisLimitTypeChanged);
    Q_PROPERTY(int totalFileSizeLimit READ getTotalFileSizeLimitMb
                   WRITE setTotalFileSizeLimitMb NOTIFY totalFileSizeLimitChanged);
    Q_PROPERTY(int concurrentFilesLimit READ getConcurrentFilesLimit
                   WRITE setConcurrentFilesLimit NOTIFY concurrentFilesLimitChanged);
    Q_PROPERTY(QString rootPath READ getRootPath WRITE setRootPath NOTIFY rootPathChanged);

public:
    explicit ProjectSettingsNode(ProjectTreeNode* parent);

signals:
    void threadCountChanged();
    void combinedAnalysisChanged();
    void useCustomThreadCountChanged();
    void analysisLimitTypeChanged();
    void totalFileSizeLimitChanged();
    void concurrentFilesLimitChanged();
    void rootPathChanged();

public:
    void setThreadCount(int threadCount);
    void setRootPath(QString path);
    void setCombinedAnalysis(bool enable);
    void setUseCustomThreadCount(bool enable);
    void setAnalysisLimitType(AnalysisLimitType type);
    void setTotalFileSizeLimitMb(int size);
    void setConcurrentFilesLimit(int limit);

public:
    [[nodiscard]]
    const QString& getRootPath() const;
    [[nodiscard]]
    int getThreadCount() const;
    [[nodiscard]]
    bool getCombinedAnalysis() const;
    [[nodiscard]]
    bool hasCustomThreadCount() const;
    [[nodiscard]]
    AnalysisLimitType getAnalysisLimitType() const;
    [[nodiscard]]
    int getTotalFileSizeLimitMb() const;
    [[nodiscard]]
    int getConcurrentFilesLimit() const;

private:
    int threadCount_{1u};
    bool combinedAnalysis_{true};
    bool useCustomThreadCount_{false};

    AnalysisLimitType analysisLimitType_{};
    int totalFileSizeLimitMb_{1024};
    int concurrentFilesLimit_{10};

    QString rootPath_;
};

} // namespace bv

