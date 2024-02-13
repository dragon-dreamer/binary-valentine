#include "QtStdTypeConverter.h"

#include <exception>
#include <filesystem>

namespace bv
{

std::filesystem::path QtStdTypeConverter::toPath(
    const QString& str, core::user_errc error)
{
    try
    {
#if defined(_WIN32) || defined(_MSC_VER)
        return std::filesystem::path{str.toStdWString()};
#else
        return std::filesystem::path{str.toStdString()};
#endif
    }
    catch (const std::exception&)
    {
        std::throw_with_nested(core::user_error(error));
    }
}

QString QtStdTypeConverter::toString(
    const std::filesystem::path& path)
{
#if defined(_WIN32) || defined(_MSC_VER)
    return QString::fromStdWString(path.native());
#else
    return QString::fromStdString(path.native());
#endif
}

} //namespace bv
