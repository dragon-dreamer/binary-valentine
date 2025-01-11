#include "binary_valentine/analysis/shared_context.h"

#include "binary_valentine/common/shared_generator/common_generators.h"
#include "binary_valentine/core/embedded_resource_loader_generator.h"
#include "binary_valentine/core/value_interface.h"
#include "binary_valentine/pe/shared_generator/pe_shared_generators.h"

namespace bv::analysis
{

shared_context::shared_context(
	std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader)
	: sync_data_generators_(create_sync_shared_generators(std::move(embedded_resource_loader)))
	, sync_values_(sync_data_generators_, global_reporter_)
	, shared_generators_(create_shared_generators())
	, shared_values_(sync_values_, shared_generators_, global_reporter_)
{
	generate_sync_data();

	embedded_resource_loader_ = static_cast<core::value_provider_interface&>(sync_values_)
		.get<std::shared_ptr<core::embedded_resource_loader_interface>>()->get();
}

void shared_context::generate_sync_data()
{
	for (const auto& generator : sync_data_generators_.get_generators())
	{
		for (core::value_tag tag : generator->get_generated_tags())
		{
			[[maybe_unused]] auto value = sync_values_.get(tag);
		}
	}
}

core::async_data_generator_list shared_context::create_shared_generators()
{
	core::async_data_generator_list shared_generators;
	pe::pe_shared_generators_factory::add_pe_generators(shared_generators);
	return shared_generators;
}

core::data_generator_list shared_context::create_sync_shared_generators(
	std::shared_ptr<core::embedded_resource_loader_interface> embedded_resource_loader)
{
	core::data_generator_list shared_generators;
	common::common_generators_factory::add_common_generators(shared_generators);
	core::embedded_resource_loader_generator_factory::add_generator(
		std::move(embedded_resource_loader), shared_generators);
	return shared_generators;
}

} //namespace bv::analysis
