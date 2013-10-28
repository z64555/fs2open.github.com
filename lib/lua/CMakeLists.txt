
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
		luac.c
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
	
	set_target_properties(lua51
		PROPERTIES
			COMPILE_FLAGS _CRT_SECURE_NO_WARNINGS
	)

	set_target_properties(lua51
		PROPERTIES
			COMPILE_FLAGS "/W0"
			FOLDER "3rdparty"
			STATIC_LIBRARY_FLAGS "/LTCG"
	)
	
	SET(LUA_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "Lua 5.1 include directory")
	
	SET(LUA_LIBRARIES lua51 CACHE INTERNAL "Lua 5.1 library")
ELSE(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
	INCLUDE(util)

	MAKE_CACHE_INTERNAL(LUA_INCLUDE_DIR)
	MAKE_CACHE_INTERNAL(LUA_LIBRARIES)
ENDIF(FSO_BUILD_INCLUDED_LIBS OR NOT LUA51_FOUND)
