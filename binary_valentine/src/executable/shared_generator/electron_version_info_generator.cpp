#include "binary_valentine/executable/shared_generator/electron_version_info_generator.h"

#include <memory>
#include <regex>
#include <stdexcept>
#include <string_view>
#include <utility>

#define BOOST_JSON_NO_LIB
#include <boost/json/parse.hpp>
#include <boost/json/value_to.hpp>
#include <boost/lexical_cast.hpp>

#include "binary_valentine/core/async_data_generator.h"
#include "binary_valentine/core/data_generator_list.h"
#include "binary_valentine/core/embedded_resource_loader_interface.h"
#include "binary_valentine/core/transparent_hash.h"
#include "binary_valentine/executable/shared_data/recent_electron_version_info.h"
#include "binary_valentine/https/https_request_helper.h"

namespace bv::executable
{

class electron_version_info_generator final
	: public core::async_data_generator_base<electron_version_info_generator>
{
public:
	static constexpr std::string_view generator_name = "pe_electron_version_info_generator";

	[[nodiscard]]
	boost::asio::awaitable<core::typed_value_ptr<recent_electron_version_info>> generate(
		const std::shared_ptr<core::embedded_resource_loader_interface>& embedded_resource_loader) const
	{
		auto result = core::make_value<recent_electron_version_info>();

		auto npm_response = co_await https::https_request_helper::get("https://registry.npmjs.org/electron/latest");

		auto version_str = boost::json::value_to<std::string>(
			boost::json::parse(npm_response).at("version"));

		static const std::regex version_regex{ "(\\d+)\\.(\\d+)\\.(\\d+)",
			std::regex_constants::ECMAScript | std::regex_constants::optimize };

		std::smatch match;
		if (std::regex_match(version_str, match, version_regex))
		{
			result->get_value().recent_stable_version = {
				.major = boost::lexical_cast<std::uint32_t>(match[1].str()),
				.minor = boost::lexical_cast<std::uint32_t>(match[2].str()),
				.build = boost::lexical_cast<std::uint32_t>(match[3].str()),
			};
			result->get_value().recent_stable_version.full_string = std::move(version_str);
		}
		else
		{
			throw std::runtime_error("Unable to parse latest NPM Electron version");
		}

		co_return result;
	}
};

void electron_version_info_generator_factory::add_generator(core::async_data_generator_list& generators)
{
	generators.add<electron_version_info_generator>();
}

} //namespace bv::executable
