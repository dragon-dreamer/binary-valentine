#pragma once

#include <utility>

#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_cache.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/core/data_generator_list.h"

namespace bv::output { class common_report_interface; }

namespace bv::core
{

class [[nodiscard]] value_provider final : public value_provider_interface
{
public:
	explicit value_provider(const data_generator_list& generators,
		output::common_report_interface& report)
		: generators_(generators)
		, report_(report)
	{
	}

	explicit value_provider(value_cache&& cache,
		const data_generator_list& generators,
		output::common_report_interface& report)
		: cache_(std::move(cache))
		, generators_(generators)
		, report_(report)
	{
	}

	[[nodiscard]]
	virtual std::optional<const value_interface*> try_get(value_tag tag) const override;
	[[nodiscard]]
	virtual const value_interface* get(value_tag tag) override;
	virtual void set(value_tag tag, value_ptr value) override;
	virtual bool remove(value_tag tag) override;
	[[nodiscard]]
	virtual bool empty() const noexcept override;

	template<typename Func>
	void remove_if(Func&& func)
	{
		cache_.remove_if(func);
	}

	[[nodiscard]]
	value_cache& get_cache() noexcept
	{
		return cache_;
	}

	[[nodiscard]]
	output::common_report_interface& get_report() noexcept
	{
		return report_;
	}

private:
	value_cache cache_;
	const data_generator_list& generators_;
	output::common_report_interface& report_;
};

} //namespace bv::core
