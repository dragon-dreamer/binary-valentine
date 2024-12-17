#include "binary_valentine/core/data_generator_list.h"

#include <cassert>
#include <utility>

#include "binary_valentine/core/async_data_generator_interface.h"
#include "binary_valentine/core/core_error.h"
#include "binary_valentine/core/combined_data_generator_interface.h"
#include "binary_valentine/core/data_generator_interface.h"

namespace bv::core
{

template<typename GeneratorInterface>
void data_generator_list_base<GeneratorInterface>::add(std::span<const value_tag> tags,
	std::unique_ptr<const GeneratorInterface>&& generator)
{
	assert(!tags.empty());
	const auto* generator_ptr = generator.get();
	generators_.emplace_back(std::move(generator));
	if constexpr (is_rule_class_aware)
	{
		for (auto tag : tags)
			tag_to_generator_.emplace(tag, generator_ptr);
	}
	else
	{
		for (auto tag : tags)
		{
			if (!tag_to_generator_.try_emplace(tag, generator_ptr).second)
				throw core_error(core_errc::duplicate_generator);
		}
	}
}

template class data_generator_list_base<async_data_generator_interface>;
template class data_generator_list_base<data_generator_interface>;
template class data_generator_list_base<combined_data_generator_interface>;

} //namespace bv::core
