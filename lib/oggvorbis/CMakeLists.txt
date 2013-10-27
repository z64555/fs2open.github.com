
IF(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(OGG REQUIRED ogg)
	pkg_check_modules(THEORA REQUIRED theora)
	pkg_check_modules(VORBIS REQUIRED vorbis)
	pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
ELSEIF (WIN32)
	# We have precompiled libs for windows
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

	add_library(theora STATIC IMPORTED)
	set_target_properties(theora
		PROPERTIES
			IMPORTED_LOCATION lib/theora_static.lib
			IMPORTED_LOCATION_DEBUG lib/theora_static_d.lib
	)

	SET(THEORA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "THEORA include directory")

	SET(THEORA_LIBRARY theora CACHE INTERNAL "Theora library")
ENDIF(UNIX)

