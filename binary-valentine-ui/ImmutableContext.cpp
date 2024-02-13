#include "ImmutableContext.h"

#include <memory>
#include <stdexcept>
#include <string_view>

#include <QFile>

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

        const auto arr = file.readAll();
        return std::string(arr.constData(), arr.length());
    }
};
} //namespace

const analysis::immutable_context& ImmutableContext::getImmutableContext()
{
    static const analysis::immutable_context context{
        std::make_shared<QtResourceLoader>()
    };
    return context;
}

const bv::string::resource_provider_interface& ImmutableContext::getLocalizedResources()
{
    static constexpr std::string_view language("en");
    static const auto resources = bv::string::embedded_resource_loader{}.load(language);
    return *resources;
}

} // namespace bv
