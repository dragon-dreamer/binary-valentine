#include "binary_valentine/common/shared_generator/program_path_generator.h"

#include <string_view>
#include <utility>

#include <boost/dll/runtime_symbol_info.hpp>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/common/shared_data/program_path.h"

namespace bv::common
{

class program_path_generator final
	: public core::data_generator_base<program_path_generator>
{
public:
	static constexpr std::string_view generator_name = "program_path_generator";
	static constexpr std::string_view resource_subpath = "data";

	[[nodiscard]]
	core::typed_value_ptr<program_path> generate() const
	{
		auto executable_path = boost::dll::program_location();
		std::filesystem::path resource_path(executable_path.native());
		resource_path.remove_filename();
		resource_path /= resource_subpath;
		return core::make_value<program_path>(std::move(resource_path));
	}
};

void program_path_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<program_path_generator>();
}

} //namespace bv::common
