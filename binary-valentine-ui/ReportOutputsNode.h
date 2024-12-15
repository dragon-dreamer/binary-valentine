#pragma once

#include <QLatin1StringView>
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QtQmlIntegration>

#include "ProjectTreeNode.h"

namespace bv
{

class ReportOutputsNode final : public ProjectTreeNode
{
public:
    static constexpr QLatin1StringView fileFormatKey{"fileFormat"};
    static constexpr QLatin1StringView filePathKey{"path"};

    enum FileFormat
    {
        FileFormatText,
        FileFormatSarif,
        FileFormatHtml
    };
    Q_ENUM(FileFormat);

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(OutputReports);
    QML_UNCREATABLE("Cannot be created from QML");
    Q_PROPERTY(bool terminalOutput READ useTerminalOutput
                   WRITE enableTerminalOutput NOTIFY terminalOutputChanged);

public:
    explicit ReportOutputsNode(ProjectTreeNode* parent);

public:
    Q_INVOKABLE QVariantList getOutputFiles() const;
    Q_INVOKABLE QVariantMap addNewOutputFile();
    Q_INVOKABLE void deleteOutputFile(int index);
    Q_INVOKABLE void setOutputFileFormat(int index, FileFormat fileFormat);
    Q_INVOKABLE void setOutputFilePath(int index, QString path);

    [[nodiscard]]
    bool useTerminalOutput() const noexcept
    {
        return terminalOutput_;
    }

public:
    void enableTerminalOutput(bool enable);
    void addOutputFile(FileFormat format, QString path);

signals:
    void terminalOutputChanged();

private:
    bool terminalOutput_{};
    QVariantList outputFiles_;
};

} // namespace bv

