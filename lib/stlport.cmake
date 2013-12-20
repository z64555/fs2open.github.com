
SET(USE_STLPORT ON)

INCLUDE(ExternalProject)

IF(MSVC60)
	SET(args msvc6 -p win98)
ELSE(MSVC60)
	SET(args msvc8)
ENDIF(MSVC60)

IF(MSVC_USE_RUNTIME_DLL)
	SET(STL_RUNTIME_LIBRARY "--with-dynamic-rtl")
ELSE(MSVC_USE_RUNTIME_DLL)
	SET(STL_RUNTIME_LIBRARY "--with-static-rtl")
ENDIF(MSVC_USE_RUNTIME_DLL)

ExternalProject_Add(stlport
	PREFIX stlport
	# Downloading
	URL http://sourceforge.net/projects/stlport/files/STLport/STLport-5.2.1/STLport-5.2.1.tar.gz # Full path or URL of source
	URL_MD5 b20ace9f4a487b2614dfbb205bee112c               # MD5 checksum of file at URL
	# Configuring, we need to use cmd as configure calls exit
	CONFIGURE_COMMAND cmd /C configure ${args} ${STL_RUNTIME_LIBRARY}
	# Building
	BUILD_COMMAND cd build/lib && nmake clean install
	BUILD_IN_SOURCE 1
	# Installing
	INSTALL_COMMAND ""
)
	
set_target_properties(stlport
	PROPERTIES
		FOLDER "3rdparty"
)
	
ExternalProject_Get_Property(stlport source_dir)

SET(STLPORT_INCLUDE_DIRS "${source_dir}/stlport" CACHE INTERNAL)
SET(STLPORT_INCLUDE_LIB_DIRS "${source_dir}/lib" CACHE INTERNAL)
