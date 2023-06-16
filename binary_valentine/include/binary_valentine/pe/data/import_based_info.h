#pragma once

#include <string>

#include "pe_bliss2/core/file_header.h"

namespace bv::pe
{

struct import_based_info
{
	bool is_service = false;
	bool is_dotnet_native = false;
	bool is_native_universal_windows_platform = false;
};

} //namespace bv::pe
