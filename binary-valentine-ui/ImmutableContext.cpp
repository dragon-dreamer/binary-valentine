#include "ImmutableContext.h"

#include <memory>
#include <stdexcept>
#include <string_view>

#include <QFile>

#include "binary_valentine/analysis/shared_context.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/string/embedded_resource_loader.h"

namespace bv
{

namespace
{
class QtResourceLoader final : public core::embedded_resource_loader_interface
{
public:
    virtual std::string load_file(std::string_view path) const override
    {
        QFile file(":/binary-valentine-ui/data/" + QString::fromStdString(std::string(path)));
        if (!file.open(QIODeviceBase::ReadOnly | QIODeviceBase::ExistingOnly))
            throw std::runtime_error("Unable to open built-in resource file " + std::string(path));

        std::string result;
        result.resize(file.size());
        if (result.size() != file.read(result.data(), result.size()))
            throw std::runtime_error("Unable to read built-in resource file " + std::string(path));
        return result;
    }
};
} //namespace

analysis::shared_context ImmutableContext::createSharedContext()
{
    return analysis::shared_context(std::make_shared<QtResourceLoader>());
}

const analysis::immutable_context& ImmutableContext::getImmutableContext()
{
    static const analysis::immutable_context context;
    return context;
}

const bv::string::resource_provider_interface& ImmutableContext::getLocalizedResources()
{
    static constexpr std::string_view language("en");
    static const auto resources = bv::string::embedded_resource_loader{}.load(language);
    return *resources;
}

} // namespace bv
