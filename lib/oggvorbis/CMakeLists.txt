
IF(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(OGG REQUIRED ogg)
	pkg_check_modules(THEORA REQUIRED theora)
	pkg_check_modules(VORBIS REQUIRED vorbis)
	pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
ELSEIF (WIN32)
	# We have precompiled libs for windows
	add_library(ogg STATIC IMPORTED GLOBAL)
	set_target_properties(ogg
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/ogg_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/ogg_static_d.lib
	)

	add_library(vorbis STATIC IMPORTED GLOBAL)
	set_target_properties(vorbis
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/vorbis_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/vorbis_static_d.lib
	)

	add_library(vorbisfile STATIC IMPORTED GLOBAL)
	set_target_properties(vorbisfile
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/vorbisfile_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/vorbisfile_static_d.lib
	)

	SET(VORBIS_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Vorbis include directory")
	SET(OGG_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Ogg include directory")

	SET(OGGVORBIS_LIBRARIES ogg vorbis vorbisfile CACHE INTERNAL "OggVorbis library")

	add_library(theora STATIC IMPORTED GLOBAL)
	set_target_properties(theora
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/theora_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/theora_static_d.lib
	)

	SET(THEORA_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "THEORA include directory")

	SET(THEORA_LIBRARIES theora CACHE INTERNAL "Theora library")
ENDIF(UNIX)

