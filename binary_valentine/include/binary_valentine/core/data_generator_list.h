#pragma once

#include <memory>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include "binary_valentine/core/value_interface.h"

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

public:
	[[nodiscard]]
	const generator_interface_type* get(value_tag tag) const;

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
	std::vector<std::unique_ptr<const generator_interface_type>> generators_;
	std::unordered_map<value_tag, const generator_interface_type*> tag_to_generator_;
};

using data_generator_list = data_generator_list_base<data_generator_interface>;
using async_data_generator_list = data_generator_list_base<async_data_generator_interface>;
using combined_data_generator_list = data_generator_list_base<combined_data_generator_interface>;

} //namespace bv::core
