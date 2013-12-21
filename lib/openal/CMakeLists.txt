
IF(FSO_USE_OPENALSOFT)
	INCLUDE(ExternalProject)
	MESSAGE(STATUS "Using OpenAL Soft for OpenAL support")
	ExternalProject_Add(OpenALsoft
		PREFIX OpenALsoft
		# Downloading
		URL http://kcat.strangesoft.net/openal-releases/openal-soft-1.15.1.tar.bz2 # Full path or URL of source
		URL_MD5 ea83dec3b9655a27d28e7bc7cae9cd71               # MD5 checksum of file at URL
		
		CMAKE_CACHE_ARGS -DUTILS:BOOL=OFF -DEXAMPLES:BOOL=OFF -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>/\\\${BUILD_TYPE}
	)

	set_target_properties(OpenALsoft
		PROPERTIES
			FOLDER "3rdparty"
	)
	
	ExternalProject_Get_Property(OpenALsoft INSTALL_DIR)
	
	
ELSE(FSO_USE_OPENALSOFT)
	IF(NOT WIN32)
		FIND_PACKAGE(OpenAL REQUIRED)
	ENDIF(NOT WIN32)

	# We have precompiled libs for windows
	IF (WIN32)
		MESSAGE(STATUS "Could not find OpenAL libraries on system, using included pre-built libraries")
		
		add_library(openal STATIC IMPORTED GLOBAL)
		set_target_properties(openal
			PROPERTIES
				IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/win32/openal32.lib
		)

		SET(OPENAL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "OpenAL include directory")

		SET(OPENAL_LIBRARY openal CACHE INTERNAL "OpenAL library")
	ELSE(WIN32)
		INCLUDE(util)

		MAKE_CACHE_INTERNAL(OPENAL_INCLUDE_DIR)
		MAKE_CACHE_INTERNAL(OPENAL_LIBRARY)
	ENDIF(WIN32)
ENDIF(FSO_USE_OPENALSOFT)
