#include "binary_valentine/analysis/result_report_file.h"

#include "binary_valentine/string/encoding.h"

namespace bv::analysis
{

result_report_file::result_report_file(std::string_view path, result_report_file_type type)
	: path_(string::utf8_to<std::filesystem::path::string_type>
		::convert(path))
	, type_(type)
{
}

result_report_file::result_report_file(
	std::filesystem::path&& path, result_report_file_type type)
	: path_(std::move(path))
	, type_(type)
{
}

} //namespace bv::analysis
