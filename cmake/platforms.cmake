
SET(PLATFORM_INCLUDES "" CACHE INTERNAL "Compiler specific includes")
SET(PLATFORM_LBRARIES "" CACHE INTERNAL "Compiler specific libraries")

IF(WIN32)
	INCLUDE(win32)
ELSEIF(UNIX)
	INCLUDE(unix)
ELSE(WIN32)
	MESSAGE(STATUS "This platform is not supported, good luck!")
ENDIF(WIN32)
