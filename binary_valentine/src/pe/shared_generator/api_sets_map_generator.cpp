#include "binary_valentine/pe/shared_generator/api_sets_map_generator.h"

#include <memory>
#include <string_view>
#include <utility>

#include "binary_valentine/common/xml_loader.h"
#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/pe/shared_data/api_sets.h"

namespace bv::pe
{

class api_sets_map_generator final
	: public core::data_generator_base<api_sets_map_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_api_sets_map_generator";
	static constexpr std::string_view all_sets_file_name = "apisets.xml";

	[[nodiscard]]
	core::typed_value_ptr<api_sets> generate(
		const std::shared_ptr<core::embedded_resource_loader_interface>& embedded_resource_loader) const
	{
		const auto xml = common::xml_loader::load_xml(
			std::string_view{ embedded_resource_loader->load_file(all_sets_file_name) });

		auto result = core::make_value<api_sets>();
		auto& sets = result->get_value();
		for (const auto& child : xml.child("root").children("set"))
			add_api_set(child, sets);

		sets.build_dll_to_api_set_map();
		return result;
	}

private:
	static void add_api_set(const pugi::xml_node& api_node,
		api_sets& sets)
	{
		sets.add_api_set(api_node.attribute("name").value(),
			api_node.attribute("dll").value());
	}
};

void api_sets_map_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<api_sets_map_generator>();
}

} //namespace bv::pe
