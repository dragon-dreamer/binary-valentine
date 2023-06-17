#include "binary_valentine/core/async_value_provider.h"

#include "binary_valentine/core/async_data_generator_interface.h"
#include "binary_valentine/output/internal_report_arg_names.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{

boost::asio::awaitable<const value_interface*> async_value_provider::get_async(
	value_tag tag)
{
	auto result = sync_provider_.get_cache().get(tag);
	if (result.initialized)
		co_return result.value;

	auto gen_ptr = async_generators_.get(tag);
	if (gen_ptr)
	{
		try
		{
			co_await gen_ptr->generate_data(*this);
			result = sync_provider_.get_cache().get(tag);
		}
		catch (...)
		{
			sync_provider_.get_cache().set(tag, {});
			sync_provider_.get_report().log_noexcept(output::report_level::critical,
				output::reports::generator_exception,
				output::current_exception_arg(),
				output::named_arg(output::arg::generator_name, gen_ptr->get_name()));
			co_return result.value;
		}

		assert(result.initialized);
		co_return result.value;
	}

	co_return sync_provider_.get(tag);
}

} //namespace bv::core
