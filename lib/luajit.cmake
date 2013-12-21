INCLUDE(ExternalProject)
INCLUDE(util)

IF(WIN32)
	IF(MSVC)
		SET(BUILD_COMMAND cd src && msvcbuild)
	ELSEIF(MINGW)
		SET(BUILD_COMMAND mingw32-make)
	ELSEIF(CYGWIN)
		SET(BUILD_COMMAND make)
	ELSE(MSVC)
		MESSAGE(ERROR "Unknown compiler! Cannot build luajit!")
	ENDIF(MSVC)
ELSE(WIN32)
	SET(BUILD_COMMAND make)
ENDIF(WIN32)

IF(WIN32)
	EP_CHECK_FILE_EXISTS("src/luajit/src/lua51.dll" BUILD_COMMAND luajit test_compile.cmake "${BUILD_COMMAND}" "src/luajit")
ELSE(WIN32)
	EP_CHECK_FILE_EXISTS("src/luajit/src/libluajit.a" BUILD_COMMAND luajit test_compile.cmake "${BUILD_COMMAND}" "src/luajit")
ENDIF(WIN32)

ExternalProject_Add(luajit
	PREFIX luajit
	# Downloading
	URL http://luajit.org/download/LuaJIT-2.0.2.tar.gz # Full path or URL of source
	URL_MD5 112dfb82548b03377fbefbba2e0e3a5b               # MD5 checksum of file at URL
	# Configuring
	CONFIGURE_COMMAND ""
	# Building
	BUILD_COMMAND ${BUILD_COMMAND}
	BUILD_IN_SOURCE 1
	# Installing
	INSTALL_COMMAND ""
)

set_target_properties(luajit
	PROPERTIES
		FOLDER "3rdparty"
)

ExternalProject_Get_Property(luajit source_dir)

SET(LUA_INCLUDE_DIR ${source_dir}/src CACHE INTERNAL "LuaJIT include directory")

add_library(luajit_lib SHARED IMPORTED GLOBAL)
ADD_DEPENDENCIES(luajit_lib luajit)

IF(WIN32)
	set_target_properties(luajit_lib
		PROPERTIES
			IMPORTED_LOCATION ${source_dir}/src/lua51.dll
			IMPORTED_IMPLIB ${source_dir}/src/lua51.lib
	)
	
	INSTALL(FILES ${source_dir}/src/lua51.dll
			DESTINATION ${BINARY_DESTINATION}
	)
		
	SET(TARGET_COPY_FILES ${TARGET_COPY_FILES} "${source_dir}/src/lua51.dll" CACHE INTERNAL "")
ELSE(WIN32)
	set_target_properties(luajit_lib
		PROPERTIES
			IMPORTED_LOCATION ${source_dir}/src/libluajit.a
	)
ENDIF(WIN32)

SET(LUA_LIBRARIES luajit_lib CACHE INTERNAL "LuaJIT library")
