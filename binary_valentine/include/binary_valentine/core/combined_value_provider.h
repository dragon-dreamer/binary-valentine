#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "binary_valentine/core/combined_data_generator_interface.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_cache.h"
#include "binary_valentine/core/value_provider_interface.h"

namespace bv::output { class common_report_interface; }

namespace bv::core
{


class [[nodiscard]] combined_value_provider final : public value_provider_interface
{
public:
	explicit combined_value_provider(
		const combined_data_generator_list& generators,
		std::vector<std::shared_ptr<value_provider_interface_base>> individual_providers,
		output::common_report_interface& report)
		: combined_generators_(generators)
		, report_(report)
		, individual_providers_(std::move(individual_providers))
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

	[[nodiscard]]
	const std::vector<std::shared_ptr<value_provider_interface_base>>&
		get_individual_providers() const noexcept
	{
		return individual_providers_;
	}

private:
	value_cache cache_;
	const combined_data_generator_list& combined_generators_;
	output::common_report_interface& report_;
	std::vector<std::shared_ptr<value_provider_interface_base>> individual_providers_;
};

} //namespace bv::core
