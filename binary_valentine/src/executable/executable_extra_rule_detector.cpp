#include "binary_valentine/executable/executable_extra_rule_detector.h"

#include <algorithm>

#include "binary_valentine/core/subject_entity_interface.h"
#include "binary_valentine/rule_class.h"

namespace bv::executable
{

boost::asio::awaitable<bool> executable_extra_rule_detector::detect(
	const core::subject_entity_interface& entity,
	core::entity_stream_provider_interface& /* stream_provider */,
	core::value_cache& /* values */,
	std::vector<core::rule_class_type>& rules) const
{
	auto has_executable = std::ranges::find(rules,
		static_cast<core::rule_class_type>(rule_class_type::executable)) != rules.cend();
	if (has_executable)
		co_return true;

	if (entity.is_regular_file()
		&& (entity.get_status().permissions()
			& (std::filesystem::perms::group_exec
				| std::filesystem::perms::others_exec
				| std::filesystem::perms::owner_exec))
		!= std::filesystem::perms::none)
	{
		rules.emplace_back(static_cast<core::rule_class_type>(rule_class_type::executable));
		co_return true;
	}

	co_return false;
}

} //namespace bv::executable
