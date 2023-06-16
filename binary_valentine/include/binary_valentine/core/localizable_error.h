#pragma once

#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

namespace bv::core
{

class localizable_error : public std::system_error
{
public:
	using arg_type = std::pair<std::string, std::string>;
	using arg_list_type = std::vector<arg_type>;

public:
	template<typename... Args>
	explicit localizable_error(std::error_code ec, Args&&... args)
		: std::system_error(ec)
		, string_id_(code().message())
		, args_{ std::forward<Args>(args)... }
	{
	}

	[[nodiscard]]
	std::string_view get_error_string_id() const noexcept
	{
		return string_id_;
	}

	[[nodiscard]]
	const arg_list_type& get_args() const noexcept
	{
		return args_;
	}

private:
	std::string string_id_;
	arg_list_type args_;
};

} //namespace bv::core
