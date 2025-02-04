#include "binary_valentine/core/combined_value_provider.h"

#include <cassert>
#include <exception>
#include <format>
#include <utility>

#include "binary_valentine/core/core_error.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{

[[nodiscard]]
std::optional<const value_interface*> combined_value_provider::try_get(value_tag tag) const
{
	auto result = cache_.get(tag);
	if (!result.initialized)
		return std::nullopt;

	return result.value;
}

const value_interface* combined_value_provider::get(value_tag tag)
{
	auto result = cache_.get(tag);
	if (!result.initialized)
	{
		auto gen_ptr = combined_generators_.get(tag);
		if (!gen_ptr)
		{
			report_.log(output::report_level::critical,
				output::reports::no_generator_for_type,
				output::named_arg(output::arg::tag_name, tag.name()));
			cache_.set(tag, {});
			return result.value;
		}

		try
		{
			gen_ptr->generate_data(individual_providers_, *this);
			result = cache_.get(tag);
		}
		catch (...)
		{
			cache_.set(tag, {});
			report_.log_noexcept(output::report_level::critical,
				output::reports::generator_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::generator_name, gen_ptr->get_name()));
			return result.value;
		}
	}

	assert(result.initialized);
	return result.value;
}

void combined_value_provider::set(value_tag tag, value_ptr value)
{
	cache_.set(tag, std::move(value));
}

bool combined_value_provider::remove(value_tag tag)
{
	return cache_.remove(tag);
}

} //namespace bv::core
