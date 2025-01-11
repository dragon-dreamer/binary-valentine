#pragma once

#include <utility>

#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/rule_class_mask.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_cache.h"
#include "binary_valentine/core/value_provider_interface.h"

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
		, detected_rules_(rule_class_mask::max)
	{
	}

	explicit value_provider(value_cache&& cache,
		const data_generator_list& generators,
		output::common_report_interface& report,
		rule_class_mask detected_rules)
		: cache_(std::move(cache))
		, generators_(generators)
		, report_(report)
		, detected_rules_(detected_rules)
	{
	}

	[[nodiscard]]
	virtual std::optional<const value_interface*> try_get(value_tag tag) const override;
	[[nodiscard]]
	virtual const value_interface* get(value_tag tag) override;
	virtual void set(value_tag tag, value_ptr value) override;
	virtual bool remove(value_tag tag) override;

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

	[[nodiscard]]
	const rule_class_mask& get_detected_rules() const noexcept
	{
		return detected_rules_;
	}

private:
	value_cache cache_;
	const data_generator_list& generators_;
	output::common_report_interface& report_;
	rule_class_mask detected_rules_;
};

} //namespace bv::core
