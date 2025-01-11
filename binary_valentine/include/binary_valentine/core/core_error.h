#pragma once

#include <system_error>
#include <type_traits>

namespace bv::core
{

class core_error : public std::system_error
{
public:
	using std::system_error::system_error;
};

enum class core_errc
{
	duplicate_generator = 1,
	duplicate_localization_ids,
	duplicate_report,
	unsupported_language,
	unable_to_make_https_request
};

std::error_code make_error_code(core_errc) noexcept;

} //namespace bv::core

namespace std
{
template<>
struct is_error_code_enum<bv::core::core_errc> : true_type {};
} //namespace std
