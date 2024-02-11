#pragma once

#include <memory>

#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"

namespace bv::core
{

class embedded_resource_loader_generator_factory final
{
public:
	embedded_resource_loader_generator_factory() = delete;
	static void add_generator(std::shared_ptr<embedded_resource_loader_interface> loader,
		core::data_generator_list& generators);
};

} //namespace bv::core
