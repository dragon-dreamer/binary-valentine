#include "binary_valentine/file/async_target_enumerator.h"

#include <algorithm>
#include <filesystem>
#include <regex>

#include "binary_valentine/analysis/analysis_plan.h"
#include "binary_valentine/core/rule_selector.h"

namespace bv::file
{

namespace
{
boost::asio::awaitable<bool> enumerate_dir_iterator(
	std::stop_token stop_token, const auto& process_file,
	const auto& path, const auto& root_path,
	const core::rule_selector& selector, const auto& entry_callback,
	auto iterator)
{
	std::error_code enumeration_errc;
	bool has_enumeration_errors = false;
	for (auto it = iterator, end = decltype(iterator){}; it != end;)
	{
		const std::filesystem::directory_entry* child{};
		try
		{
			child = &*it;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			enumeration_errc = e.code();
		}

		if (stop_token.stop_requested())
			co_return false;

		if (!child)
		{
			if (!has_enumeration_errors)
			{
				has_enumeration_errors = true;
				co_await entry_callback({
					.entry = path,
					.root_path = root_path,
					.selector = selector,
					.ec = enumeration_errc
				});
			}
		}
		else
		{
			if (child->is_regular_file())
				co_await process_file(*child);
		}

		it.increment(enumeration_errc);
		if (enumeration_errc && !has_enumeration_errors)
		{
			has_enumeration_errors = true;
			co_await entry_callback({
				.entry = path,
				.root_path = root_path,
				.selector = selector,
				.ec = enumeration_errc
			});
		}
	}
	co_return true;
}

boost::asio::awaitable<bool> enumerate_impl(
	const std::filesystem::path& path,
	const analysis::target_filter& filter,
	const core::rule_selector& selector,
	const std::filesystem::path& root_path,
	const async_target_enumerator::callback_type& callback,
	bool recursive,
	const std::stop_token& stop_token)
{
	std::error_code ec;
	auto canonical_path = std::filesystem::canonical(path, ec);
	if (ec)
	{
		co_await callback({
			.entry = path,
			.root_path = root_path,
			.selector = selector,
			.ec = ec
			});
		co_return true;
	}

	std::filesystem::directory_entry entry(canonical_path, ec);
	if (ec)
	{
		co_await callback({
			.entry = std::move(canonical_path),
			.root_path = root_path,
			.selector = selector,
			.ec = ec
		});
		co_return true;
	}

	auto process_file = [&](const std::filesystem::directory_entry& file)
		-> boost::asio::awaitable<void>
	{
		if (!filter.satisfies(file))
			co_return;

		co_await callback({
			.entry = file,
			.root_path = root_path,
			.selector = selector
		});
	};

	if (entry.is_directory())
	{
		if (recursive)
		{
			co_await enumerate_dir_iterator(stop_token, process_file,
				entry, root_path, selector, callback,
				std::filesystem::recursive_directory_iterator(entry,
				std::filesystem::directory_options::follow_directory_symlink
				| std::filesystem::directory_options::skip_permission_denied, ec));
		}
		else
		{
			co_await enumerate_dir_iterator(stop_token, process_file,
				entry, root_path, selector, callback,
				std::filesystem::directory_iterator(entry,
				std::filesystem::directory_options::follow_directory_symlink
				| std::filesystem::directory_options::skip_permission_denied, ec));
		}

		if (ec)
		{
			co_await callback({
				.entry = std::move(entry),
				.root_path = root_path,
				.selector = selector,
				.ec = ec
				});
		}
	}
	else if (entry.is_regular_file())
	{
		co_await process_file(entry);
	}

	co_return true;
}

boost::asio::awaitable<bool> enumerate_impl(
	const analysis::plan_target& target,
	const std::filesystem::path& root_path,
	const analysis::analysis_plan& plan,
	const async_target_enumerator::callback_type& callback,
	const std::stop_token& stop_token)
{
	if (stop_token.stop_requested())
		co_return false;

	const auto& selector = target.get_rule_selector()
		? target.get_rule_selector().value() : plan.get_global_rule_selector();
	if (target.get_path().is_absolute())
	{
		co_return co_await enumerate_impl(target.get_path(), target.get_target_filter(),
			selector, target.get_path(), callback, target.is_recursive(), stop_token);
	}
	else
	{
		auto path = root_path / target.get_path();
		co_return co_await enumerate_impl(path, target.get_target_filter(),
			selector, root_path, callback, target.is_recursive(), stop_token);
	}
}
} //namespace

boost::asio::awaitable<bool> async_target_enumerator::enumerate(
	const analysis::analysis_plan& plan,
	const callback_type& callback,
	std::stop_token stop_token)
{
	auto root_path = plan.get_root_path();
	if (root_path.empty())
	{
		root_path = std::filesystem::current_path();
	}
	else
	{
		std::error_code ec;
		auto canonical_root_path = std::filesystem::canonical(root_path, ec);
		if (ec)
		{
			co_await callback({
				.entry = root_path,
				.root_path = root_path,
				.selector = plan.get_global_rule_selector(),
				.ec = ec
			});
			co_return false;
		}
		root_path = std::move(canonical_root_path);
	}

	for (const auto& target : plan.get_targets())
	{
		if (!co_await enumerate_impl(target, root_path, plan, callback, stop_token))
			co_return false;
	}

	co_return true;
}

} //namespace bv::file
