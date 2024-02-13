#pragma once

#include "binary_valentine/analysis/immutable_context.h"
#include "binary_valentine/string/resource_provider_interface.h"

namespace bv
{

class ImmutableContext final
{
public:
    ImmutableContext() = delete;

    [[nodiscard]]
    static const analysis::immutable_context& getImmutableContext();

    [[nodiscard]]
    static const bv::string::resource_provider_interface& getLocalizedResources();
};

} // namespace bv
