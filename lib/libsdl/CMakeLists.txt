
IF(WIN32)
	add_library(sdl SHARED IMPORTED GLOBAL)

	set_target_properties(sdl
		PROPERTIES
			IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2.dll"
			IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2.lib"
			INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
	)

	INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2.dll"
			DESTINATION ${BINARY_DESTINATION}
	)

	SET(TARGET_COPY_FILES ${TARGET_COPY_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/lib/x86/SDL2.dll" CACHE INTERNAL "" FORCE)

	SET(SDL_LIBS sdl CACHE INTERNAL "")

	add_library(sdlmain STATIC src/SDL_windows.h src/SDL_windows_main.c)
	set_target_properties(sdlmain PROPERTIES FOLDER "3rdparty")
	target_link_libraries(sdlmain PRIVATE sdl)
ELSE(WIN32)
	# CMake can't check for SDL2 so we'll just use PkgConfig
	find_package(PkgConfig)
	INCLUDE(util)

	message("Searching SDL on Unix")
	IF(IS_APPLE)
		message("Searching SDL on Apple")
		PKG_SEARCH_MODULE(SDL2 sdl2)

		message("Searching SDL on Apple: ${SDL2_LIB}")
		IF(NOT SDL2_FOUND)
			MESSAGE(STATUS "SDL not found: Using included SDL libraries")
			SET(FSO_USE_SDL_FRAMEWORK ON) # We ship these for mac
		ENDIF(NOT SDL2_FOUND)
	ELSE(IS_APPLE)
		PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
	ENDIF(IS_APPLE)

	IF(FSO_USE_SDL_FRAMEWORK)
		message("Searching SDL on Apple in ${FSO_MAC_FRAMEWORKS}")
		find_library(SDL2_LIBRARY SDL2 PATHS "${FSO_MAC_FRAMEWORKS}")

		message("Found SDL on Apple: ${SDL2_LIBRARY}")
		add_library(sdl INTERFACE)
		target_link_libraries(sdl INTERFACE ${SDL2_LIBRARY})
	ELSE(FSO_USE_SDL_FRAMEWORK)
		PKG_CONFIG_LIB_RESOLVE(SDL2 SDL2_LIB)
		ADD_IMPORTED_LIB(sdl "${SDL2_INCLUDE_DIRS}" "${SDL2_LIB}" SHARED)
	ENDIF(FSO_USE_SDL_FRAMEWORK)

	SET(SDL_LIBS sdl CACHE INTERNAL "")
ENDIF(WIN32)
