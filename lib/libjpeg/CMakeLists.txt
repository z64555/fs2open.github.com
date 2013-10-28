
IF(NOT FSO_BUILD_INCLUDED_LIBS)
	FIND_PACKAGE(JPEG QUIET)
ENDIF(NOT FSO_BUILD_INCLUDED_LIBS)

IF (FSO_BUILD_INCLUDED_LIBS OR NOT JPEG_FOUND)
	MESSAGE(STATUS "Could not find jpeg libraries on system, using included source")

	SET(JPEG_SOURCES 
		jaricom.c
		jcapimin.c
		jcapistd.c
		jcomapi.c
		jcparam.c
		jctrans.c
		jdapimin.c
		jdapistd.c
		jdarith.c
		jdatasrc.c
		jdcoefct.c
		jdcolor.c
		jddctmgr.c
		jdhuff.c
		jdinput.c
		jdmainct.c
		jdmarker.c
		jdmaster.c
		jdmerge.c
		jdpostct.c
		jdsample.c
		jdtrans.c
		jerror.c
		jidctflt.c
		jidctfst.c
		jidctint.c
		jmemmgr.c
		jmemnobs.c
		jquant1.c
		jquant2.c
		jutils.c
		jconfig.h
		jdct.h
		jerror.h
		jinclude.h
		jmemsys.h
		jmorecfg.h
		jpegint.h
		jpeglib.h
		jversion.h)

	ADD_LIBRARY(jpeg ${JPEG_SOURCES})
	
	set_target_properties(jpeg
		PROPERTIES
			COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
	)

	set_target_properties(jpeg
		PROPERTIES
			COMPILE_FLAGS "/W0"
			FOLDER "3rdparty"
			STATIC_LIBRARY_FLAGS "/LTCG"
	)
	
	SET(JPEG_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "JPEG include directory")
	
	SET(JPEG_LIBRARIES jpeg CACHE INTERNAL "JPEG library")
ELSE(FSO_BUILD_INCLUDED_LIBS OR NOT JPEG_FOUND)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(JPEG_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(JPEG_LIBRARIES)
ENDIF(FSO_BUILD_INCLUDED_LIBS OR NOT JPEG_FOUND)
