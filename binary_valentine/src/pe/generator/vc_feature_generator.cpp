#include "binary_valentine/pe/generator/vc_feature_generator.h"

#include <string_view>
#include <utility>
#include <variant>

#include "binary_valentine/core/data_generator.h"
#include "binary_valentine/core/data_generator_list.h"

#include "pe_bliss2/debug/debug_directory.h"

namespace bv::pe
{

class vc_feature_generator final
	: public core::data_generator_base<vc_feature_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_vc_feature_generator";

	[[nodiscard]]
	core::typed_value_ptr<pe_bliss::debug::vc_feature_debug_directory_details> generate(
		const pe_bliss::debug::debug_directory_list_details& debug_dir) const
	{
		for (const auto& entry : debug_dir.get_entries())
		{
			if (entry.get_type() != pe_bliss::debug::debug_directory_type::vc_feature)
				continue;

			auto underlying_dir = entry.get_underlying_directory();
			auto* vc_feature = std::get_if<
				pe_bliss::debug::vc_feature_debug_directory_details>(
					&underlying_dir);
			if (vc_feature)
			{
				return core::make_value<
					pe_bliss::debug::vc_feature_debug_directory_details>(std::move(*vc_feature));
			}
			break;
		}

		return nullptr;
	}
};

void vc_feature_generator_factory::add_generator(core::data_generator_list& generators)
{
	generators.add<vc_feature_generator>();
}

} //namespace bv::pe
