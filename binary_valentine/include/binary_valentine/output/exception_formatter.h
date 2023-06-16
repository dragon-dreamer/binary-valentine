#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "binary_valentine/output/result_report_interface.h"

namespace bv::output
{

class [[nodiscard]] exception_formatter_interface
{
public:
	virtual ~exception_formatter_interface() = default;

	[[nodiscard]]
	virtual bool try_format(report_level level,
		std::exception_ptr exception,
		common_report_interface& report) const = 0;

	[[nodiscard]]
	virtual bool try_format(std::exception_ptr exception,
		arg_value_type& value) const = 0;
};

class [[nodiscard]] exception_formatter
{
public:
	template<typename Formatter, typename... Args>
	void add_formatter(Args&&... args)
	{
		extra_formatters_.emplace_back(
			std::make_unique<Formatter>(std::forward<Args>(args)...));
	}
	
	void format(report_level level,
		std::exception_ptr exception,
		common_report_interface& report) const;

	void format(std::exception_ptr exception,
		arg_value_type& value) const;

private:
	std::vector<std::unique_ptr<exception_formatter_interface>> extra_formatters_;
};

} //namespace bv::output
