#pragma once

#include <string>

#include "pe_bliss2/core/file_header.h"

namespace bv::pe
{

struct basic_pe_info
{
	bool is_resource_only = false;
	bool is_xbox = false;
	bool is_pre_win7ce = false;
	bool is_boot = false;
	bool is_il_only = false;
	bool is_mixed = false;
	bool is_executable = false;
	bool is_managed = false;
	bool loads_as_32bit = false;
	bool is_driver = false;
	bool has_executable_code = false;
	pe_bliss::core::file_header::machine_type machine_type{};
	std::string file_extension;
	std::string file_name;
};

} //namespace bv::pe
