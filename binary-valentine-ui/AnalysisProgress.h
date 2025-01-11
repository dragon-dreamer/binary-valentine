#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QtQmlIntegration>

namespace bv
{

class AnalysisProgress : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AnalysisProgress);
    QML_UNCREATABLE("Cannot be created from QML");

public:
    enum ProgressState
    {
        TargetAnalysis,
        CombinedAnalysis,
        WritingReports
    };
    Q_ENUM(ProgressState);

    Q_PROPERTY(bool isValid MEMBER isValid_ FINAL CONSTANT);
    Q_PROPERTY(int totalRead MEMBER totalRead_ FINAL CONSTANT);
    Q_PROPERTY(int totalAnalyzed MEMBER totalAnalyzed_ FINAL CONSTANT);
    Q_PROPERTY(QString currentAnalyzedTargetPath MEMBER currentAnalyzedTargetPath_ FINAL CONSTANT);
    Q_PROPERTY(QVariantList newFiles MEMBER newFiles_ FINAL CONSTANT);
    Q_PROPERTY(ProgressState progressState MEMBER progressState_ FINAL CONSTANT);

public:
    AnalysisProgress() = default;
    AnalysisProgress(int totalRead, int totalAnalyzed,
                     QString currentAnalyzedTargetPath,
                     ProgressState progressState);

public:
    void setNewFiles(QVariantList newFiles);

private:
    bool isValid_{};
    int totalRead_{};
    int totalAnalyzed_{};
    QString currentAnalyzedTargetPath_{};
    QVariantList newFiles_;
    ProgressState progressState_{ TargetAnalysis };
};

} //namespace bv
