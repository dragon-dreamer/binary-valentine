cmake_minimum_required(VERSION 3.15)

project(binary_valentine_cmd
	VERSION 0.1.0.0
	LANGUAGES CXX)

#//TODO
#set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
#set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
#set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
#set(Boost_USE_STATIC_RUNTIME    OFF)

#if (WIN32)
  # disable autolinking in boost
#  add_definitions( -DBOOST_ALL_NO_LIB )

  # force all boost libraries to dynamic link (we already disabled
  # autolinking, so I don't know why we need this, but we do!)
#  add_definitions( -DBOOST_ALL_DYN_LINK )
#endif()

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
endif()
