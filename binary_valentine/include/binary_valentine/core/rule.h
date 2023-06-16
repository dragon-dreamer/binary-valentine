#pragma once

#include <span>

#include "binary_valentine/core/rule_interface.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/value_helper.h"
#include "binary_valentine/output/result_report_interface.h"
#include "binary_valentine/output/rule_report_helper.h"

namespace bv::core
{

template<typename Derived>
class [[nodiscard]] rule_base : public rule_interface
{
public:
	using constructor_dependencies_type = dependencies<>;

public:
	constexpr rule_base() noexcept
		: rule_interface(Derived::rule_name)
	{
	}

private:
	virtual void run(output::entity_report_interface& report,
		std::span<const value_interface* const> dependencies) const final
	{
		using run_type = func_types_skip_first_arg<&Derived::template run<
			output::rule_report_helper<Derived>>>;

		value_helper<typename run_type::dependencies_type>
			::call_with_values(dependencies,
			[this, &report](auto&&... values)
		{
			output::rule_report_helper<Derived> reporter(report);
			return static_cast<const Derived&>(*this)
				.run(reporter, values...);
		});
	}

	[[nodiscard]]
	virtual bool is_rule_applicable(
		std::span<const value_interface* const> dependencies) const final
	{
		if constexpr (requires { &Derived::is_applicable; })
		{
			using is_applicable_type = func_types<&Derived::is_applicable>;
			return value_helper<typename is_applicable_type::dependencies_type>
				::call_with_values(dependencies, [this](auto&&... values)
			{
				return static_cast<const Derived&>(*this)
					.is_applicable(values...);
			});
		}
		else
		{
			return true;
		}
	}

	[[nodiscard]]
	virtual std::span<const output::rule_report_base> get_reports() const noexcept final
	{
		return Derived::reports;
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_prerequisite_dependencies() const noexcept final
	{
		if constexpr (requires { &Derived::is_applicable; })
		{
			using is_applicable_type = func_types<&Derived::is_applicable>;
			static constexpr auto result = value_helper<
				typename is_applicable_type::dependencies_type>::get_dependency_array();
			return result;
		}
		else
		{
			return {};
		}
	}

	[[nodiscard]]
	virtual std::span<const optional_dependency>
		get_run_dependencies() const noexcept final
	{
		using run_type = func_types_skip_first_arg<&Derived::template run<
			output::rule_report_helper<Derived>>>;
		static constexpr auto result = value_helper<
			typename run_type::dependencies_type>::get_dependency_array();
		return result;
	}
};

} //namespace bv::core
