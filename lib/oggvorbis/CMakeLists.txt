
IF(WIN32)
	FIND_PACKAGE(OggVorbis)
	FIND_PACKAGE(Theora)
ELSE(WIN32)
	FIND_PACKAGE(OggVorbis REQUIRED)
	FIND_PACKAGE(Theora REQUIRED)
ENDIF(WIN32)

# We have precompiled libs for windows
IF (NOT OGGVORBIS_FOUND AND WIN32)
	MESSAGE(STATUS "Could not find ogg vorbis libraries on system, using included pre-built libraries")

	add_library(ogg STATIC IMPORTED)
	set_target_properties(ogg
		PROPERTIES
			IMPORTED_LOCATION lib/ogg_static.lib
			IMPORTED_LOCATION_DEBUG lib/ogg_static_d.lib
	)

	add_library(vorbis STATIC IMPORTED)
	set_target_properties(vorbis
		PROPERTIES
			IMPORTED_LOCATION lib/vorbis_static.lib
			IMPORTED_LOCATION_DEBUG lib/vorbis_static_d.lib
	)

	add_library(vorbisfile STATIC IMPORTED)
	set_target_properties(vorbisfile
		PROPERTIES
			IMPORTED_LOCATION lib/vorbisfile_static.lib
			IMPORTED_LOCATION_DEBUG lib/vorbisfile_static_d.lib
	)

	SET(VORBIS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Vorbis include directory")
	SET(OGG_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Ogg include directory")

	SET(OGGVORBIS_LIBRARY ogg vorbis vorbisfile CACHE INTERNAL "OggVorbis library")
ELSE(NOT OGGVORBIS_FOUND AND WIN32)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(VORBIS_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(OGG_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(OGGVORBIS_LIBRARIES)
ENDIF(NOT OGGVORBIS_FOUND AND WIN32)

IF (NOT THEORA_FOUND AND WIN32)
	MESSAGE(STATUS "Could not find theora libraries on system, using included pre-built libraries")

	add_library(theora STATIC IMPORTED)
	set_target_properties(theora
		PROPERTIES
			IMPORTED_LOCATION lib/theora_static.lib
			IMPORTED_LOCATION_DEBUG lib/theora_static_d.lib
	)

	SET(THEORA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "THEORA include directory")

	SET(THEORA_LIBRARY theora CACHE INTERNAL "Theora library")
ELSE(NOT THEORA_FOUND AND WIN32)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(THEORA_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(THEORA_LIBRARY)
ENDIF(NOT THEORA_FOUND AND WIN32)

