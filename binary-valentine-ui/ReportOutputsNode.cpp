#include "ReportOutputsNode.h"

#include <cassert>
#include <utility>

namespace bv
{

namespace
{
constexpr QLatin1StringView defaultPath{"report.txt"};
} //namespace

ReportOutputsNode::ReportOutputsNode(ProjectTreeNode* parent)
    : ProjectTreeNode(parent, ProjectNodeTypeName::ReportOutputs)
{
}

QVariantList ReportOutputsNode::getOutputFiles() const
{
    return outputFiles_;
}

QVariantMap ReportOutputsNode::addNewOutputFile()
{
    QVariantMap file;
    file[fileFormatKey] = static_cast<int>(FileFormatText);
    file[filePathKey] = QString{defaultPath};
    file[useDefaultTemplateKey] = true;
    file[reportTemplateKey] = QString{};
    outputFiles_.emplaceBack(file);
    emit changesMade();
    return file;
}

void ReportOutputsNode::deleteOutputFile(int index)
{
    assert(index >= 0 && index < outputFiles_.size());
    outputFiles_.remove(index);
    emit changesMade();
}

void ReportOutputsNode::setOutputFileFormat(int index, FileFormat fileFormat)
{
    assert(index >= 0 && index < outputFiles_.size());
    auto map = outputFiles_.at(index).toMap();
    map[fileFormatKey] = static_cast<int>(fileFormat);
    outputFiles_[index] = std::move(map);
    emit changesMade();
}

void ReportOutputsNode::setOutputFilePath(int index, QString path)
{
    if (path.isEmpty())
        return;

    assert(index >= 0 && index < outputFiles_.size());
    auto map = outputFiles_.at(index).toMap();
    map[filePathKey] = std::move(path);
    outputFiles_[index] = std::move(map);
    emit changesMade();
}

void ReportOutputsNode::enableDefaultTemplate(int index, bool enable)
{
    assert(index >= 0 && index < outputFiles_.size());
    auto map = outputFiles_.at(index).toMap();
    map[useDefaultTemplateKey] = enable;
    outputFiles_[index] = std::move(map);
    emit changesMade();
}

void ReportOutputsNode::setReportTemplatePath(int index, QString path)
{
    if (path.isEmpty())
        return;

    assert(index >= 0 && index < outputFiles_.size());
    auto map = outputFiles_.at(index).toMap();
    map[reportTemplateKey] = path;
    outputFiles_[index] = std::move(map);
    emit changesMade();
}

void ReportOutputsNode::enableTerminalOutput(bool enable)
{
    if (terminalOutput_ != enable)
    {
        terminalOutput_ = enable;
        emit changesMade();
        emit terminalOutputChanged();
    }
}

void ReportOutputsNode::addOutputFile(FileFormat format, QString path,
                                      QString customTemplatePath)
{
    QVariantMap file;
    file[fileFormatKey] = static_cast<int>(format);
    file[filePathKey] = std::move(path);
    file[useDefaultTemplateKey] = customTemplatePath.isEmpty();
    file[reportTemplateKey] = std::move(customTemplatePath);
    outputFiles_.append(std::move(file));
}

} // namespace bv
