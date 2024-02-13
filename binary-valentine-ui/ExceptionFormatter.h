#pragma once

#include <exception>

#include <QString>

namespace bv
{

class ExceptionFormatter final
{
public:
    ExceptionFormatter() = delete;

    [[nodiscard]]
    static QString formatException(std::exception_ptr ptr);
};

} // namespace bv

