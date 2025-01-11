#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>

#include <boost/asio/awaitable.hpp>

#include "binary_valentine/core/async_value_provider_interface.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/value.h"
#include "binary_valentine/core/value_provider.h"

namespace bv::output { class common_report_interface; }
namespace avast::asio { class async_mutex; }

namespace bv::core
{

class [[nodiscard]] shared_value_provider final : public async_value_provider_interface
{
public:
	shared_value_provider(const value_provider& sync_values,
		const async_data_generator_list& generators,
		output::common_report_interface& report);
	virtual ~shared_value_provider() override;

	virtual void set(value_tag tag, value_ptr value) override;
	virtual bool remove(value_tag tag) override;

	[[nodiscard]]
	virtual std::optional<const value_interface*> try_get(value_tag tag) const override;
	[[nodiscard]]
	virtual boost::asio::awaitable<const value_interface*> get_async(value_tag tag) override;
	[[nodiscard]]
	bool can_provide(value_tag tag) const;

private:
	const value_provider& sync_values_;
	std::vector<avast::asio::async_mutex> generator_mutexes_;
	std::vector<std::uint8_t> executed_generators_;
	std::vector<value_ptr> values_;
	std::unordered_map<value_tag, std::pair<std::size_t, std::size_t>> tag_to_generator_value_;
	const async_data_generator_list& generators_;
	output::common_report_interface& report_;
};

} //namespace bv::core
