#pragma once

#include <filesystem>
#include <functional>
#include <stop_token>
#include <variant>

#include <boost/asio/awaitable.hpp>

namespace bv::analysis { class analysis_plan; }
namespace bv::core { class rule_selector; }

namespace bv::file
{

struct target_entry
{
	std::variant<std::filesystem::directory_entry,
		std::filesystem::path> entry;
	std::filesystem::path root_path;
	const core::rule_selector& selector;
	std::error_code ec{};
};

class async_target_enumerator final
{
public:
	using callback_type = std::function<
		boost::asio::awaitable<void>(target_entry&& entry)>;

public:
	async_target_enumerator() = delete;

	static boost::asio::awaitable<bool> enumerate(
		const analysis::analysis_plan& plan,
		const callback_type& callback,
		std::stop_token stop_token);
};

} //namespace bv::file
