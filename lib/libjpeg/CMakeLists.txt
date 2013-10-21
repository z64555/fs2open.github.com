
FIND_PACKAGE(JPEG QUIET)

IF (NOT JPEG_FOUND)
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

	ADD_LIBRARY(libjpeg ${JPEG_SOURCES})
	
	SET(JPEG_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "JPEG include directory")
	
	SET(JPEG_LIBRARIES libjpeg CACHE INTERNAL "JPEG library")
ENDIF(NOT JPEG_FOUND)
