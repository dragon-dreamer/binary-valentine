#pragma once

#include <compare>
#include <optional>
#include <string>
#include <tuple>

namespace bv::executable
{

enum class electron_build_type
{
	alpha,
	beta,
	stable,
};

struct electron_version_info
{
	std::uint32_t major{};
	std::uint32_t minor{};
	std::uint32_t build{};
	electron_build_type build_type{ electron_build_type::stable };
	std::uint32_t pre_release_version{};
	std::string full_string;

	[[nodiscard]]
	constexpr auto operator<=>(const electron_version_info& other) const noexcept
	{
		return std::tie(major, minor, build, build_type, pre_release_version)
			<=> std::tie(other.major, other.minor,
				other.build, other.build_type, other.pre_release_version);
	}
};

struct electron_app_info
{
	std::string v8_version_string;
	std::optional<electron_version_info> electron_version;
};

} //namespace bv::executable
