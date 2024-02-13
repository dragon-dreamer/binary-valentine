#include "VersionInfo.h"

#include "binary_valentine/version.h"

namespace bv
{

VersionInfo::VersionInfo(QObject *parent) : QObject(parent) {}

QString VersionInfo::getToolFullName() const
{
    return QString("%1 %2.%3.%4").arg(QLatin1StringView(bv::version::tool_name),
                                      QString::number(bv::version::tool_version[0]),
                                      QString::number(bv::version::tool_version[1]),
                                      QString::number(bv::version::tool_version[2]));
}

QString VersionInfo::getVersion() const
{
    return QString("%1.%2.%3").arg(QString::number(bv::version::tool_version[0]),
                                   QString::number(bv::version::tool_version[1]),
                                   QString::number(bv::version::tool_version[2]));
}

QString VersionInfo::getToolName() const
{
    return QString::fromLatin1(bv::version::tool_name.data(),
                               bv::version::tool_name.size());
}

QString VersionInfo::getAuthorName() const
{
    return "Denis T";
}

QString VersionInfo::getDocsWebsiteUrl() const
{
    return QLatin1StringView(bv::version::tool_website) + "/docs/?from=ui";
}

QString VersionInfo::getWebsiteUrl() const
{
    return QLatin1StringView(bv::version::tool_website);
}

QString VersionInfo::getGithubUrl() const
{
    return "https://github.com/dragon-dreamer/binary-valentine";
}

QString VersionInfo::getRuleInfoUrl(QString ruleUid) const
{
    return QString("%1%2%3").arg(QLatin1StringView(bv::version::tool_website),
                                 QLatin1StringView(bv::version::rule_info_uri),
                                 ruleUid);
}

QString VersionInfo::getQtVersion() const
{
    return QT_VERSION_STR;
}

} //namespace bv
