
IF(NOT FSO_BUILD_INCLUDED_LIBS)
	FIND_PACKAGE(Lua51 QUIET)
ENDIF(NOT FSO_BUILD_INCLUDED_LIBS)

IF (FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
	MESSAGE(STATUS "Could not find lua libraries on system, using included source")
	
	SET(LUA51_SOURCES
		lapi.c
		lauxlib.c
		lbaselib.c
		lcode.c
		ldblib.c
		ldebug.c
		ldo.c
		ldump.c
		lfunc.c
		lgc.c
		linit.c
		liolib.c
		llex.c
		lmathlib.c
		lmem.c
		loadlib.c
		lobject.c
		lopcodes.c
		loslib.c
		lparser.c
		lstate.c
		lstring.c
		lstrlib.c
		ltable.c
		ltablib.c
		ltm.c
		lua.c
		lundump.c
		lvm.c
		lzio.c
		print.c
		lapi.h
		lauxlib.h
		lcode.h
		ldebug.h
		ldo.h
		lfunc.h
		lgc.h
		llex.h
		llimits.h
		lmem.h
		lobject.h
		lopcodes.h
		lparser.h
		lstate.h
		lstring.h
		ltable.h
		ltm.h
		lua.h
		luaconf.h
		lualib.h
		lundump.h
		lvm.h
		lzio.h
		)

	ADD_LIBRARY(lua51 ${LUA51_SOURCES})
	
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
	
	set_target_properties(lua51
		PROPERTIES
			COMPILE_DEFINITIONS _CRT_SECURE_NO_WARNINGS
	)

	set_target_properties(lua51
		PROPERTIES
			FOLDER "3rdparty"
	)
	
	IF(MSVC)
		set_target_properties(lua51
			PROPERTIES
				STATIC_LIBRARY_FLAGS "/LTCG"
		)
	ENDIF(MSVC)
	
	SET(LUA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "Lua 5.1 include directory")
	
	SET(LUA_LIBRARIES lua51 CACHE INTERNAL "Lua 5.1 library")

	IF(FSO_COPY_EXE AND BUILD_SHARED_LIBS)
		INSTALL(
			TARGETS lua51
			RUNTIME DESTINATION ${BINARY_DESTINATION}
			LIBRARY DESTINATION ${BINARY_DESTINATION}
			CONFIGURATIONS Debug Release
			)
	ENDIF(FSO_COPY_EXE AND BUILD_SHARED_LIBS)
ELSE(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(LUA_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(LUA_LIBRARIES)
ENDIF(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
