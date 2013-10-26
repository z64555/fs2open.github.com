
FIND_PACKAGE(ZLIB QUIET)

IF (NOT ZLIB_FOUND)
	MESSAGE(STATUS "Could not find zlib libraries on system, using included source")
	
	SET(ZLIB_SOURCES
		adler32.c
		compress.c
		crc32.c
		deflate.c
		infback.c
		inffast.c
		inflate.c
		inftrees.c
		trees.c
		uncompr.c
		zutil.c
		crc32.h
		deflate.h
		inffast.h
		inffixed.h
		inflate.h
		inftrees.h
		trees.h
		zconf.h
		zlib.h
		zutil.h)

	ADD_LIBRARY(zlib ${ZLIB_SOURCES})
	
	SET(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "ZLIB include directory")
	
	SET(ZLIB_LIBRARIES zlib CACHE INTERNAL "ZLIB library")
ELSE(NOT ZLIB_FOUND)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(ZLIB_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(ZLIB_LIBRARIES)
ENDIF(NOT ZLIB_FOUND)
