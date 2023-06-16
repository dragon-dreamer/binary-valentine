#pragma once

#include <exception>
#include <optional>

#include "pe_bliss2/resources/manifest.h"

namespace bv::pe
{

enum class manifest_type
{
	no_manifest,
	application,
	assembly
};

enum class external_manifest_load_result
{
	does_not_exist,
	loaded,
	too_big,
	exists,
	error
};

struct manifest_info
{
	std::optional<pe_bliss::resources::native_manifest_details> manifest;
	manifest_type type{};
	bool has_embedded_manifest{};
	external_manifest_load_result external_result{};
	std::exception_ptr manifest_load_error{};
};

} //namespace bv::pe
