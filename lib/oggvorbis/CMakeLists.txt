
IF(UNIX AND NOT APPLE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(OGG REQUIRED ogg)
	pkg_check_modules(THEORA REQUIRED theora)
	pkg_check_modules(VORBIS REQUIRED vorbis)
	pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
	
	INCLUDE(util)
	PKG_CONFIG_LIB_RESOLVE(OGG OGG_LIBS)
	PKG_CONFIG_LIB_RESOLVE(THEORA THEORA_LIBS)
	PKG_CONFIG_LIB_RESOLVE(VORBIS VORBIS_LIBS)
	PKG_CONFIG_LIB_RESOLVE(VORBISFILE VORBISFILE_LIBS)

	MAKE_CACHE_INTERNAL(OGG_LIBS)
	MAKE_CACHE_INTERNAL(THEORA_LIBS)
	MAKE_CACHE_INTERNAL(VORBIS_LIBS)
	MAKE_CACHE_INTERNAL(VORBISFILE_LIBS)
ELSEIF (WIN32)
	# We have precompiled libs for windows
	add_library(ogg STATIC IMPORTED GLOBAL)
	set_target_properties(ogg
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/ogg_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/ogg_static_d.lib
			INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
	)
	SET(OGG_LIBS ogg CACHE INTERNAL "OggVorbis library")

	add_library(vorbis STATIC IMPORTED GLOBAL)
	set_target_properties(vorbis
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/vorbis_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/vorbis_static_d.lib
			INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
	)
	SET(VORBIS_LIBS vorbis CACHE INTERNAL "OggVorbis library")

	add_library(vorbisfile STATIC IMPORTED GLOBAL)
	set_target_properties(vorbisfile
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/vorbisfile_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/vorbisfile_static_d.lib
			INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
	)
	SET(VORBISFILE_LIBS vorbisfile CACHE INTERNAL "OggVorbis library")

	add_library(theora STATIC IMPORTED GLOBAL)
	set_target_properties(theora
		PROPERTIES
			IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/theora_static.lib
			IMPORTED_LOCATION_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib/theora_static_d.lib
			INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include
	)
	SET(THEORA_LIBS theora CACHE INTERNAL "Theora library")
ELSEIF(APPLE)
	# We also have precompiled libs for windows
	add_library(ogg STATIC IMPORTED GLOBAL)
	set_target_properties(ogg
		PROPERTIES
			IMPORTED_LOCATION ${FSO_MAC_FRAMEWORKS}/Ogg.framework/Ogg
			INTERFACE_INCLUDE_DIRECTORIES "${FSO_MAC_FRAMEWORKS}/Ogg.framework/Headers"
	)
	SET(OGG_LIBS ogg CACHE INTERNAL "OggVorbis library")

	add_library(vorbis STATIC IMPORTED GLOBAL)
	set_target_properties(vorbis
		PROPERTIES
			IMPORTED_LOCATION ${FSO_MAC_FRAMEWORKS}/Vorbis.framework/Vorbis
			INTERFACE_INCLUDE_DIRECTORIES "${FSO_MAC_FRAMEWORKS}/Vorbis.framework/Headers"
	)
	SET(VORBIS_LIBS vorbis CACHE INTERNAL "OggVorbis library")

	SET(VORBISFILE_LIBS "" CACHE INTERNAL "OggVorbis library")

	add_library(theora STATIC IMPORTED GLOBAL)
	set_target_properties(theora
		PROPERTIES
			IMPORTED_LOCATION ${FSO_MAC_FRAMEWORKS}/Theora.framework/Theora
			INTERFACE_INCLUDE_DIRECTORIES "${FSO_MAC_FRAMEWORKS}/Theora.framework/Headers"
	)
	SET(THEORA_LIBS theora CACHE INTERNAL "Theora library")
ELSE(UNIX AND NOT APPLE)
	MESSAGE(FATAL_ERROR "Unconfigured oggvorbis platform, please report!")
ENDIF(UNIX AND NOT APPLE)

