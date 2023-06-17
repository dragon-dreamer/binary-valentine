#pragma once

#include <cassert>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

#include "binary_valentine/core/combined_rule_interface.h"
#include "binary_valentine/core/combined_value_helper.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/value_provider_interface.h"
#include "binary_valentine/output/result_report_interface.h"

namespace bv::analysis { class combined_analysis_plan; }

namespace bv::core
{

template<typename Derived>
class [[nodiscard]] combined_rule_base : public combined_rule_interface
{
public:
	using combined_rule_interface::combined_rule_interface;

	using dependencies_type = combined_dependencies<>;
	using constructor_dependencies_type = dependencies<>;

	constexpr combined_rule_base() noexcept
		: combined_rule_interface(Derived::rule_name)
	{
	}

	[[nodiscard]]
	static bool applies_to(const individual_values_span_type::value_type& provider)
	{
		const auto* selector = provider->try_get<std::reference_wrapper<
			const rule_selector>>();
		assert(!!selector);
		for (const auto& report : Derived::reports)
		{
			if (selector->get().is_enabled(report))
				return true;
		}
		return false;
	}

private:
	virtual void run_rule(
		individual_values_span_type individual_values,
		value_provider_interface& combined_values) const final
	{
		combined_value_helper<typename Derived::dependencies_type>
			::template call_with_values<Derived>(individual_values, combined_values,
				[this](auto&&... values)
		{
			return static_cast<const Derived&>(*this)
				.run(std::forward<decltype(values)>(values)...);
		});
	}

	[[nodiscard]]
	virtual std::span<const output::rule_report_base> get_reports() const noexcept final
	{
		return Derived::reports;
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency> get_individual_dependencies() const noexcept final
	{
		static constexpr auto deps = combined_value_helper<typename Derived::dependencies_type>
			::template get_dependencies<false>();
		return deps;
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency> get_combined_dependencies() const noexcept final
	{
		static constexpr auto deps = combined_value_helper<typename Derived::dependencies_type>
			::template get_dependencies<true>();
		return deps;
	}
};

} //namespace bv::core
