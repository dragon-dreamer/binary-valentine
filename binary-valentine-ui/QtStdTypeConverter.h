#pragma once

#include <filesystem>

#include <QString>

#include "binary_valentine/core/user_error.h"

namespace bv
{

class QtStdTypeConverter final
{
public:
    QtStdTypeConverter() = delete;

    [[nodiscard]]
    static std::filesystem::path toPath(const QString& str, core::user_errc error);
    [[nodiscard]]
    static QString toString(const std::filesystem::path& path);
};

} //namespace bv
