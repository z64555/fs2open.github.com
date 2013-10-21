
FIND_PACKAGE(OggVOrbis)

# We have precompiled libs for windows
IF (NOT OGGVORBIS_FOUND AND WIN32)

	SET(VORBIS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Vorbis include directory")
	SET(OGG_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include CACHE INTERNAL "Ogg include directory")
	
	SET(OGGVORBIS_LIBRARIES ${CMAKE_CURRENT_LIST_DIR}/lib/ogg_static.lib ${CMAKE_CURRENT_LIST_DIR}/lib/theora_static.lib
		${CMAKE_CURRENT_LIST_DIR}/lib/vorbis_static.lib ${CMAKE_CURRENT_LIST_DIR}/lib/vorbisfile_static.lib
		CACHE INTERNAL "Ogg vorbis library files")
ENDIF(NOT OGGVORBIS_FOUND AND WIN32)
