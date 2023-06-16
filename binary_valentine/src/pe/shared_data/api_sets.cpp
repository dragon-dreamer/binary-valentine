#include "binary_valentine/pe/shared_data/api_sets.h"

namespace bv::pe
{

bool reduce_api_set_name(std::string_view& dll_name)
{
	if (is_api_set_name(dll_name))
	{
		auto last_dash = dll_name.find_last_of('-');
		if (last_dash != std::string::npos)
		{
			dll_name = dll_name.substr(0, last_dash);
			return true;
		}
	}
	return false;
}

bool is_api_set_name(std::string_view dll_name)
{
	return dll_name.starts_with("ext-") || dll_name.starts_with("api-");
}

} //namespace bv::pe
