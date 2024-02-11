#pragma once

#include <functional>
#include <memory>

namespace bv::string
{
class resource_provider_interface;
} //namespace bv::string

namespace bv::core
{
class subject_entity_interface;
} //namespace bv::core

namespace bv::output
{

class entity_report_interface;
class exception_formatter;

using realtime_report_creator_type = std::function<
	std::shared_ptr<entity_report_interface>(
		const std::shared_ptr<const bv::core::subject_entity_interface>& /* entity */,
		const exception_formatter& /* formatter */,
		const string::resource_provider_interface& /* resources */)>;

} //namespace bv::output
