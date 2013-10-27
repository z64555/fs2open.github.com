
FIND_PACKAGE(PNG QUIET)

IF (NOT PNG_FOUND)
	MESSAGE(STATUS "Could not find png libraries on system, using included source")
	
	SET(PNG_SOURCES
		png.c
		pngerror.c
		pngget.c
		pngmem.c
		pngpread.c
		pngread.c
		pngrio.c
		pngrtran.c
		pngrutil.c
		pngset.c
		pngtrans.c
		pngwio.c
		pngwrite.c
		pngwtran.c
		pngwutil.c
		png.h
		pngconf.h
		pngdebug.h
		pnginfo.h
		pnglibconf.h
		pngpriv.h
		pngstruct.h)

	ADD_LIBRARY(png ${PNG_SOURCES})
	
	set_target_properties(png
		PROPERTIES
			COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
			COMPILE_DEFINITIONS_DEBUG PNG_DEBUG=1
	)

	set_target_properties(png
		PROPERTIES
			COMPILE_FLAGS "/W0"
			FOLDER "3rdparty"
			STATIC_LIBRARY_FLAGS "/LTCG"
	)
	
	INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
	
	SET(PNG_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "PNG include directory")
	
	SET(PNG_LIBRARIES png zlib CACHE INTERNAL "PNG library")
ELSE(NOT PNG_FOUND)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(PNG_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(PNG_LIBRARIES)
ENDIF(NOT PNG_FOUND)
