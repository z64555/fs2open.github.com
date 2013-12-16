
INCLUDE(ExternalProject)

IF(${CMAKE_SYSTEM_VERSION} VERSION_LESS 4.10)
	SET(STL_PLATFORM "win95")
ELSEIF(${CMAKE_SYSTEM_VERSION} VERSION_LESS 5.1)
	SET(STL_PLATFORM "win98")
ELSE(${CMAKE_SYSTEM_VERSION} VERSION_LESS 4.10)
	SET(STL_PLATFORM "winxp")
ENDIF(${CMAKE_SYSTEM_VERSION} VERSION_LESS 4.10)

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
	# Configuring
	CONFIGURE_COMMAND configure msvc6 -p ${STL_PLATFORM} ${STL_RUNTIME_LIBRARY}
	# Building
	BUILD_COMMAND ""
	BUILD_IN_SOURCE 1
	# Installing
	INSTALL_COMMAND ""
)
	
set_target_properties(stlport
	PROPERTIES
		FOLDER "3rdparty"
)
