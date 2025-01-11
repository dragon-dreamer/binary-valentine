#include "binary_valentine/core/shared_value_provider.h"

#include <atomic>

#include "avast/async_mutex.hpp"

#include "binary_valentine/core/async_data_generator.h"
#include "binary_valentine/core/core_error.h"
#include "binary_valentine/output/internal_report_messages.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::core
{

shared_value_provider::shared_value_provider(
	const value_provider& sync_values,
	const async_data_generator_list& generators,
	output::common_report_interface& report)
	: sync_values_(sync_values)
	, generator_mutexes_(generators.get_generators().size())
	, executed_generators_(generators.get_generators().size())
	, generators_(generators)
	, report_(report)
{
	std::size_t total_values{};
	for (const auto& generator : generators.get_generators())
		total_values += generator->get_generated_tags().size();

	tag_to_generator_value_.reserve(total_values);
	values_.resize(total_values);

	std::size_t value_index{};
	std::size_t generator_index{};
	for (const auto& generator : generators.get_generators())
	{
		for (auto tag : generator->get_generated_tags())
		{
			if (!tag_to_generator_value_.try_emplace(tag, generator_index, value_index).second)
				throw core_error(core_errc::duplicate_generator);

			++value_index;
		}
		++generator_index;
	}
}

shared_value_provider::~shared_value_provider() = default;

std::optional<const value_interface*> shared_value_provider::try_get(value_tag tag) const
{
	if (auto value = sync_values_.try_get(tag); value.has_value())
		return *value;

	auto it = tag_to_generator_value_.find(tag);
	if (it == tag_to_generator_value_.cend())
		return {};

	const auto [gen_index, value_index] = it->second;
	const std::uint8_t& is_generator_executed = executed_generators_[gen_index];
	std::atomic_ref<const std::uint8_t> is_generator_executed_ref(is_generator_executed);
	if (is_generator_executed_ref.load(std::memory_order_acquire))
		return values_[value_index].get();

	return {};
}

boost::asio::awaitable<const value_interface*> shared_value_provider::get_async(value_tag tag)
{
	if (auto value = sync_values_.try_get(tag); value.has_value())
		co_return *value;

	const auto [gen_index, value_index] = tag_to_generator_value_.at(tag);
	std::uint8_t& is_generator_executed = executed_generators_[gen_index];
	std::atomic_ref<std::uint8_t> is_generator_executed_ref(is_generator_executed);
	if (!is_generator_executed_ref.load(std::memory_order_acquire))
	{
		avast::asio::async_mutex& mutex = generator_mutexes_[gen_index];

		const auto lock = co_await mutex.async_scoped_lock(boost::asio::use_awaitable);
		if (is_generator_executed_ref.load(std::memory_order_acquire))
			co_return values_[value_index].get();

		static constexpr rule_class_mask all_rules(rule_class_mask::max);
		auto gen_ptr = generators_.get(tag, all_rules);
		if (gen_ptr)
		{
			try
			{
				co_await gen_ptr->generate_data(*this);
			}
			catch (...)
			{
				report_.log_noexcept(output::report_level::critical,
					output::reports::generator_exception,
					output::current_exception_arg(),
					output::named_arg(output::arg::generator_name, gen_ptr->get_name()));
			}
		}

		is_generator_executed_ref.store(1u, std::memory_order_release);
	}

	co_return values_[value_index].get();
}

bool shared_value_provider::can_provide(value_tag tag) const
{
	return tag_to_generator_value_.contains(tag)
		|| sync_values_.try_get(tag).has_value();
}

void shared_value_provider::set(value_tag tag, value_ptr value)
{
	const auto value_index = tag_to_generator_value_.at(tag).second;
	values_[value_index] = std::move(value);
}

bool shared_value_provider::remove(value_tag /* tag */)
{
	throw std::runtime_error("Unsupported");
}

} //namespace bv::core
