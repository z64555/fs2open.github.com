
IF(NOT WIN32)
	FIND_PACKAGE(OpenAL REQUIRED)
ENDIF(NOT WIN32)

# We have precompiled libs for windows
IF (WIN32)
	MESSAGE(STATUS "Could not find OpenAL libraries on system, using included pre-built libraries")
	
	add_library(openal STATIC IMPORTED GLOBAL)
	set_target_properties(openal
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/libs/win32/openal32.lib
	)

	SET(OPENAL_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "OpenAL include directory")

	SET(OPENAL_LIBRARY openal CACHE INTERNAL "OpenAL library")
ELSE(WIN32)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(OPENAL_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(OPENAL_LIBRARY)
ENDIF(WIN32)
