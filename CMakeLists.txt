
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

SET(FSO_VERSION_MAJOR 3)
SET(FSO_VERSION_MINOR 7)
SET(FSO_VERSION_RELEASE 1)

PROJECT(FSO)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "Available configurations" FORCE)

### Define options and editable variables in the section below:
IF (WIN32)
	OPTION(FSO_USE_SPEECH "Use Windows specific text-to-speach libraries" OFF)
	
	OPTION(FSO_USE_VOICEREC "Enable voice recognition support" OFF)

	OPTION(FSO_BUILD_FRED2 "Build FRED2 binary" OFF)
ENDIF(WIN32)

OPTION(FSO_COPY_EXE "Copy executables to FS2PATH when the build is done" ON)

OPTION(FSO_BUILD_WXFRED2 "Build wxFRED2 binary" OFF)

SET(FSO_INSTRUCTION_SET "" CACHE STRING "The instruction set enhancement to use, possible options are SSE, SSE2 and AVX")

SET(FSO_FS2_PATH $ENV{FS2PATH} CACHE STRING "The path to FreeSpace 2, used for copying the binaries after building")

IF (NOT FSO_INSTRUCTION_SET STREQUAL "SSE" AND NOT FSO_INSTRUCTION_SET STREQUAL "SSE2"
	AND NOT FSO_INSTRUCTION_SET STREQUAL "AVX" AND NOT FSO_INSTRUCTION_SET STREQUAL "")

	MESSAGE(WARNING "Unrecognized instruction set '${FSO_INSTRUCTION_SET}'! Defaulting to no optionization")
	
	SET(FSO_INSTRUCTION_SET "")
ENDIF()

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
INCLUDE(toolchain)
INCLUDE(platforms)

INCLUDE(headers)

ADD_SUBDIRECTORY(lib)

ADD_SUBDIRECTORY(code)

ADD_SUBDIRECTORY(freespace2)
