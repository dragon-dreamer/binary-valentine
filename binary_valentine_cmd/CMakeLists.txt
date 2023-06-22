cmake_minimum_required(VERSION 3.15)

project(binary_valentine_cmd
	VERSION 0.1.0.0
	LANGUAGES CXX)

find_package(Boost 1.78 REQUIRED
	COMPONENTS program_options)

add_executable(binary_valentine_cmd)

include(../external/pe_bliss2/cmake/library_options.cmake)
set_output_dirs(binary_valentine_cmd)
set_msvc_runtime_library(binary_valentine_cmd "${BINARY_VALENTINE_MSVC_RUNTIME_LIBRARY}")

target_compile_features(binary_valentine_cmd PRIVATE cxx_std_20)

target_sources(binary_valentine_cmd
	PRIVATE
		src/main.cpp
)

target_link_libraries(binary_valentine_cmd
	PRIVATE
		binary_valentine_lib
		Boost::program_options
)

add_dependencies(binary_valentine_cmd binary_valentine_data)

if(MSVC)
	target_compile_options(binary_valentine_cmd PRIVATE "/MP")
	target_sources(binary_valentine_cmd
		PRIVATE
			win32/manifest.manifest
			win32/binary_valentine_cmd.rc
			win32/resource.h
	)
endif()
