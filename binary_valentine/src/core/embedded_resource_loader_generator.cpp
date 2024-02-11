#include "binary_valentine/core/embedded_resource_loader_generator.h"

#include "binary_valentine/common/shared_data/program_path.h"
#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/file_embedded_resource_loader.h"

namespace bv::core
{

class embedded_resource_loader_generator final
	: public core::data_generator_base<embedded_resource_loader_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_embedded_resource_loader_generator";

	explicit embedded_resource_loader_generator(
		std::shared_ptr<embedded_resource_loader_interface> loader) noexcept
		: loader_(std::move(loader))
	{
	}

	[[nodiscard]]
	core::typed_value_ptr<std::shared_ptr<embedded_resource_loader_interface>> generate(
		const common::program_path& program_path) const
	{
		if (!loader_)
		{
			return core::make_value<std::shared_ptr<embedded_resource_loader_interface>>(
				std::make_unique<file_embedded_resource_loader>(program_path.resource_path));
		}

		return core::make_value<std::shared_ptr<embedded_resource_loader_interface>>(loader_);
	}

private:
	std::shared_ptr<embedded_resource_loader_interface> loader_;
};

void embedded_resource_loader_generator_factory::add_generator(
	std::shared_ptr<embedded_resource_loader_interface> loader,
	core::data_generator_list& generators)
{
	generators.add<embedded_resource_loader_generator>(std::move(loader));
}

} //namespace bv::core
