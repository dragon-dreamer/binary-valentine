#include "binary_valentine/pe/shared_generator/not_recommended_imports_generator.h"

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>

#include "binary_valentine/common/xml_loader.h"
#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/pe/shared_data/api_sets.h"
#include "binary_valentine/pe/shared_data/not_recommended_imports.h"

namespace bv::pe
{

class not_recommended_imports_generator final
	: public core::data_generator_base<not_recommended_imports_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_not_recommended_imports_generator";
	static constexpr std::string_view not_recommended_winapi_file_name
		= "not_recommended_winapi.xml";

	using string_to_flags_map_type = std::unordered_map<std::string, std::uint32_t,
		core::transparent_hash, std::equal_to<>>;

	[[nodiscard]]
	core::typed_value_ptr<not_recommended_imports> generate(
		const std::shared_ptr<core::embedded_resource_loader_interface>& embedded_resource_loader,
		const api_sets& sets) const
	{
		return load_not_recommended_imports(embedded_resource_loader, sets);
	}

private:
	static core::typed_value_ptr<not_recommended_imports> load_not_recommended_imports(
		const std::shared_ptr<core::embedded_resource_loader_interface>& embedded_resource_loader,
		const api_sets& sets)
	{
		const auto xml = common::xml_loader::load_xml(
			std::string_view{ embedded_resource_loader->load_file(not_recommended_winapi_file_name) });

		const string_to_flags_map_type attr_name_to_flag{
			{ "deprecated", api_flags::deprecated },
			{ "internal", api_flags::internal },
			{ "dangerous", api_flags::dangerous },
			{ "insecure", api_flags::insecure },
			{ "xbox_only", api_flags::xbox_only }
		};

		auto result = core::make_value<not_recommended_imports>();
		auto& imports = result->get_value();
		for (const auto& child : xml.child("root").children("api"))
			add_api(child, sets, attr_name_to_flag, imports);

		return result;
	}

	static void add_api(const pugi::xml_node& api_node,
		const api_sets& sets,
		const string_to_flags_map_type& attr_name_to_flag,
		not_recommended_imports& imports)
	{
		std::uint32_t flags{};
		for (const auto& attr : api_node.attributes())
		{
			if (auto it = attr_name_to_flag.find(std::string_view(attr.name()));
				it != attr_name_to_flag.end())
			{
				if (std::string_view(attr.value()) == "1")
					flags |= it->second;
			}
		}

		auto& api = imports.add_api(api_node.attribute("name").value(),
			static_cast<api_flags::value>(flags),
			api_node.child("mitigation").text().as_string());

		for (const auto& os : api_node.children("os"))
			api.add_min_os_version(os.text().as_string());

		for (const auto& dll : api_node.children("dll"))
			imports.add_dll(api, dll.text().as_string());

		enrich_dlls_from_api_sets(sets, api_node, imports, api);
	}

	static void enrich_dlls_from_api_sets(const api_sets& sets,
		const pugi::xml_node& api_node,
		not_recommended_imports& imports,
		not_recommended_api& api)
	{
		//Add all related to the core library API sets.
		//This will result in a bit slower analysis compared to
		//cherry-picked API sets by function names (and also more memory usage),
		//but from the other hand, we do not risk missing any API sets,
		//and this is easier than trying to figure out which API sets
		//include which functions.
		for (const auto& dll : api_node.children("dll"))
		{
			std::string_view dll_name(dll.text().as_string());
			if (!is_api_set_name(dll_name))
			{
				if (auto sets_for_dll = sets.get_api_sets_for_dll(dll_name);
					sets_for_dll)
				{
					for (const auto& api_set : *sets_for_dll)
						imports.try_add_api_set(api, api_set);
				}
			}
		}
	}
};

void not_recommended_imports_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<not_recommended_imports_generator>();
}

} //namespace bv::pe
