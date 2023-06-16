cmake_minimum_required(VERSION 3.15)

project(binary_valentine_data)

set_source_files_properties(data_files PROPERTIES SYMBOLIC ON)

add_custom_command(
	OUTPUT data_files
	COMMAND "${CMAKE_COMMAND}" -E make_directory
		"${CMAKE_BINARY_DIR}/bin/$<CONFIG>/data"
	COMMAND "${CMAKE_COMMAND}" -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/all_winapi.xml"
		"${CMAKE_CURRENT_SOURCE_DIR}/apisets.xml"
		"${CMAKE_CURRENT_SOURCE_DIR}/not_recommended_winapi.xml"
		"${CMAKE_BINARY_DIR}/bin/$<CONFIG>/data"
)

add_custom_target(binary_valentine_data
	DEPENDS data_files
)
