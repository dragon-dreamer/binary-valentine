cmake_minimum_required(VERSION 3.15)

cmake_policy(SET CMP0091 NEW)

project(binary_valentine
	VERSION 0.1.0.0
	LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (MSVC)
	option(BINARY_VALENTINE_STATIC_MSVC_RUNTIME "Link all binaries with MSVC runtime statically" ON)
	if (BINARY_VALENTINE_STATIC_MSVC_RUNTIME)
		option(PE_BLISS_STATIC_MSVC_RUNTIME "" ON)
		set(Boost_USE_STATIC_RUNTIME ON)
		set(BINARY_VALENTINE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	else ()
		option(PE_BLISS_STATIC_MSVC_RUNTIME "" OFF)
		set(Boost_USE_STATIC_RUNTIME OFF)
		set(BINARY_VALENTINE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	endif ()
endif ()

option(FMT_OS "" OFF)
add_subdirectory(external/fmt-10.1.1)

add_subdirectory(external/utfcpp)
add_subdirectory(external/pe_bliss2)

add_subdirectory(binary_valentine)
add_subdirectory(binary_valentine_cmd)
add_subdirectory(data)

option(BINARY_VALENTINE_WITH_UI "Build QT-based GUI in addition to the command line tool" OFF)
if (BINARY_VALENTINE_WITH_UI)
	add_subdirectory(binary-valentine-ui)
endif ()

# Trim __FILE__ macros in some of the cryptopp cpp files
file(TO_NATIVE_PATH "${CMAKE_CURRENT_LIST_DIR}" CMAKE_CURRENT_LIST_DIR_NATIVE)
target_compile_options(cryptopp PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:/d1trimfile:${CMAKE_CURRENT_LIST_DIR_NATIVE}">)
