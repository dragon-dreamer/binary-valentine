#include "binary_valentine/core/core_error.h"

#include <system_error>

namespace
{

struct core_error_category : std::error_category
{
	const char* name() const noexcept override
	{
		return "bv/core_error";
	}

	std::string message(int ev) const override
	{
		using enum bv::core::core_errc;
		switch (static_cast<bv::core::core_errc>(ev))
		{
		case duplicate_generator:
			return "Duplicator generator";
		case duplicate_localization_ids:
			return "Duplicate localized string IDs";
		case duplicate_report:
			return "Duplicate rule report";
		case unsupported_language:
			return "Unsupported language";
		default:
			return {};
		}
	}
};

const core_error_category core_error_category_instance;

} //namespace

namespace bv::core
{

std::error_code make_error_code(core_errc e) noexcept
{
	return { static_cast<int>(e), core_error_category_instance };
}

} //namespace bv::core
