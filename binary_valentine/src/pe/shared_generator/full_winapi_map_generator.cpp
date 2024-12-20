#include "binary_valentine/pe/shared_generator/full_winapi_map_generator.h"

#include <memory>
#include <string_view>
#include <utility>

#include "binary_valentine/common/xml_loader.h"
#include "binary_valentine/core/async_data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/pe/shared_data/winapi_library_list.h"

namespace bv::pe
{

class full_winapi_map_generator final
	: public core::async_data_generator_base<full_winapi_map_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_full_winapi_map_generator";
	static constexpr std::string_view all_winapi_file_name = "all_winapi.xml";

	[[nodiscard]]
	boost::asio::awaitable<core::typed_value_ptr<full_winapi_library_map_type>> generate(
		const std::shared_ptr<core::embedded_resource_loader_interface>& embedded_resource_loader) const
	{
		const auto xml = common::xml_loader::load_xml(
			std::string_view{ embedded_resource_loader->load_file(all_winapi_file_name) });

		auto result = core::make_value<full_winapi_library_map_type>();
		auto& imports = result->get_value();
		for (const auto& child : xml.child("root").children("api"))
			add_api(child, imports);

		co_return result;
	}

private:
	static void add_api(const pugi::xml_node& api_node,
		full_winapi_library_map_type& imports)
	{
		auto& api = imports.add_api(api_node.attribute("name").value());
		for (const auto& dll : api_node.children("dll"))
			imports.add_dll(api, dll.text().as_string());
	}
};

void full_winapi_map_generator_factory::add_generator(core::async_data_generator_list& generators)
{
	generators.add<full_winapi_map_generator>();
}

} //namespace bv::pe
