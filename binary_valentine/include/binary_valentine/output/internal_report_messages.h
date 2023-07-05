#pragma once

#include <string_view>

namespace bv::output::reports
{

constexpr std::string_view generator_exception = "generator_exception";
constexpr std::string_view rule_exception = "rule_exception";
constexpr std::string_view no_generator_for_type = "no_generator_for_type";
constexpr std::string_view inaccessible_entity = "inaccessible_entity";
constexpr std::string_view analysis_exception = "analysis_exception";
constexpr std::string_view entity_load_exception = "entity_load_exception";
constexpr std::string_view entity_processing_error = "entity_processing_error";
constexpr std::string_view system_error_description = "system_error_description";
constexpr std::string_view user_error_description = "user_error_description";
constexpr std::string_view exception_description = "exception_description";
constexpr std::string_view description_not_available = "description_not_available";
constexpr std::string_view target_enumeration_error = "target_enumeration_error";
constexpr std::string_view explicit_stop_requested = "explicit_stop_requested";
constexpr std::string_view analysis_finished_message = "analysis_finished_message";
constexpr std::string_view analysis_interrupted_message = "analysis_interrupted_message";
constexpr std::string_view writing_report_error = "writing_report_error";

constexpr std::string_view progress_analysis_completed = "progress_analysis_completed";
constexpr std::string_view progress_analysis_started = "progress_analysis_started";
constexpr std::string_view progress_loaded = "progress_loaded";
constexpr std::string_view progress_load_started = "progress_load_started";
constexpr std::string_view progress_combined_analysis_started = "progress_combined_analysis_started";
constexpr std::string_view progress_combined_analysis_completed = "progress_combined_analysis_completed";
constexpr std::string_view progress_target_skipped_unsupported = "progress_target_skipped_unsupported";
constexpr std::string_view progress_target_skipped_filtered = "progress_target_skipped_filtered";

} //namespace bv::output::reports
