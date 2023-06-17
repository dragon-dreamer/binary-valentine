find_path(URING_INCLUDE_DIR liburing.h)
find_library(URING_LIBRARY uring)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(uring DEFAULT_MSG URING_LIBRARY URING_INCLUDE_DIR)

if (URING_FOUND)
	if (NOT TARGET uring)
		add_library(uring UNKNOWN IMPORTED)
	endif()
	
	set_target_properties(uring
		PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${URING_INCLUDE_DIR}"
		IMPORTED_LINK_INTERFACE_LANGUAGES "C"
		IMPORTED_LOCATION "${URING_LIBRARY}")
	mark_as_advanced(URING_LIBRARY)
endif()
