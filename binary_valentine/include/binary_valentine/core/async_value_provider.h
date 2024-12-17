#pragma once

#include <utility>
#include <vector>

#include "binary_valentine/core/async_value_provider_interface.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/rule_class_mask.h"
#include "binary_valentine/core/value_provider.h"

namespace bv::core
{

class [[nodiscard]] async_value_provider final : public async_value_provider_interface
{
public:
	explicit async_value_provider(value_cache&& cache,
		const async_data_generator_list& async_generators,
		const data_generator_list& generators,
		output::common_report_interface& report,
		const rule_class_mask& detected_rules)
		: sync_provider_(std::move(cache), generators, report, detected_rules)
		, async_generators_(async_generators)
	{
	}

	[[nodiscard]]
	virtual value_provider_interface& get_sync_provider() noexcept override
	{
		return sync_provider_;
	}

	[[nodiscard]]
	virtual std::optional<const value_interface*> try_get(value_tag tag) const override
	{
		return sync_provider_.try_get(tag);
	}

	virtual void set(value_tag tag, value_ptr value) override
	{
		sync_provider_.set(tag, std::move(value));
	}

	virtual bool remove(value_tag tag) override
	{
		return sync_provider_.remove(tag);
	}

	[[nodiscard]]
	virtual bool empty() const noexcept override
	{
		return sync_provider_.empty();
	}

	template<typename Func>
	void remove_if(Func&& func)
	{
		sync_provider_.remove_if(func);
	}

	[[nodiscard]]
	virtual boost::asio::awaitable<const value_interface*> get_async(value_tag tag) override;

private:
	value_provider sync_provider_;
	const async_data_generator_list& async_generators_;
};

} //namespace bv::core
