#pragma once

#include <memory>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/core/rule_class_mask.h"

namespace bv::core
{

class async_data_generator_interface;
class data_generator_interface;
class combined_data_generator_interface;

template<typename GeneratorInterface>
class [[nodiscard]] data_generator_list_base
{
public:
	using generator_interface_type = GeneratorInterface;

	static constexpr bool is_rule_class_aware
		= !std::is_same_v<generator_interface_type, combined_data_generator_interface>;

public:
	[[nodiscard]]
	const generator_interface_type* get(value_tag tag,
		const rule_class_mask& rule_classes) const
		requires(is_rule_class_aware)
	{
		auto [from, to] = tag_to_generator_.equal_range(tag);
		while (from != to)
		{
			if (from->second->should_run_on(rule_classes))
				return from->second;
			++from;
		}
		return nullptr;
	}

	[[nodiscard]]
	const generator_interface_type* get(value_tag tag) const
		requires(!is_rule_class_aware)
	{
		auto gen_it = tag_to_generator_.find(tag);
		return gen_it == tag_to_generator_.end() ? nullptr : gen_it->second;
	}

	template<typename Generator, typename... Args>
	void add(Args&&... args)
	{
		add(Generator::tags(),
			std::make_unique<Generator>(std::forward<Args>(args)...));
	}

private:
	void add(std::span<const value_tag> tags,
		std::unique_ptr<const generator_interface_type>&& generator);

private:
	using map_type = std::conditional_t<is_rule_class_aware,
		std::unordered_multimap<value_tag, const generator_interface_type*>,
		std::unordered_map<value_tag, const generator_interface_type*>>;

	std::vector<std::unique_ptr<const generator_interface_type>> generators_;
	map_type tag_to_generator_;
};

using data_generator_list = data_generator_list_base<data_generator_interface>;
using async_data_generator_list = data_generator_list_base<async_data_generator_interface>;
using combined_data_generator_list = data_generator_list_base<combined_data_generator_interface>;

} //namespace bv::core
