
IF(NOT FSO_BUILD_INCLUDED_LIBS AND UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(JANSSON REQUIRED jansson)
	
	INCLUDE(util)
	PKG_CONFIG_LIB_RESOLVE(JANSSON)

	MAKE_CACHE_INTERNAL(JANSSON_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(JANSSON_LIBRARIES)
ELSE(NOT FSO_BUILD_INCLUDED_LIBS AND UNIX)	
	SET(JANSSON_SOURCES
		dump.c
		error.c
		hashtable.c
		load.c
		memory.c
		pack_unpack.c
		strbuffer.c
		strconv.c
		utf.c
		value.c
		hashtable.h
		jansson.h
		jansson_config.h
		jansson_private.h
		strbuffer.h
		utf.h)

	ADD_LIBRARY(jansson ${JANSSON_SOURCES})
	
	set_target_properties(jansson
		PROPERTIES
			COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
	)

	set_target_properties(jansson
		PROPERTIES
			FOLDER "3rdparty"
	)
	
	IF(MSVC)
		set_target_properties(jansson
			PROPERTIES
				STATIC_LIBRARY_FLAGS "/LTCG"
		)
	ENDIF(MSVC)
	
	SET(JANSSON_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "jansson include directory")
	
	SET(JANSSON_LIBRARIES jansson CACHE INTERNAL "jansson library")
ENDIF(NOT FSO_BUILD_INCLUDED_LIBS AND UNIX)
