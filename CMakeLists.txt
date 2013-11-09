
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(FSO_VERSION_MAJOR 3)
SET(FSO_VERSION_MINOR 7)
SET(FSO_VERSION_BUILD 1)
SET(FSO_VERSION_REVISION 0)

SET(FSO_PROJECT_DESCRIPTION "Open source project based on the original FreeSpace 2 source code.")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "Executable output path")

PROJECT(FSO)

set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "Available configurations" FORCE)

### Define options and editable variables in the section below:
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Default build configuration used." FORCE)
	message(WARNING "CMAKE_BUILD_TYPE was not specified, defaulting to Release configuration.")
endif()
	
IF (WIN32)
	OPTION(FSO_USE_SPEECH "Use Windows specific text-to-speach libraries" OFF)
	
	OPTION(FSO_USE_VOICEREC "Enable voice recognition support" OFF)

	OPTION(FSO_BUILD_FRED2 "Build FRED2 binary" OFF)
ENDIF(WIN32)

OPTION(FSO_BUILD_WXFRED2 "Build wxFRED2 binary" OFF)

OPTION(FSO_BUILD_INCLUDED_LIBS "Build and use the included libraries istead of using the system headers and libraries" OFF)

OPTION(FSO_BUILD_PACKAGE "Build a distribution pacakge" OFF)

SET(FSO_INSTRUCTION_SET "" CACHE STRING "The instruction set enhancement to use, possible options are SSE, SSE2 and AVX")

SET(FSO_FS2_PATH $ENV{FS2PATH} CACHE STRING "The path to FreeSpace 2, used for copying the binaries after building")

IF (NOT FSO_INSTRUCTION_SET STREQUAL "SSE" AND NOT FSO_INSTRUCTION_SET STREQUAL "SSE2"
	AND NOT FSO_INSTRUCTION_SET STREQUAL "AVX" AND NOT FSO_INSTRUCTION_SET STREQUAL "")

	MESSAGE(WARNING "Unrecognized instruction set '${FSO_INSTRUCTION_SET}'! Defaulting to no optionization")
	
	SET(FSO_INSTRUCTION_SET "")
ENDIF()

IF(NOT IS_DIRECTORY ${FSO_FS2_PATH})
	MESSAGE(STATUS "The path '${FSO_FS2_PATH}' of the variable FSO_FS2_PATH does not point to a directory!")
	SET(FSO_COPY_EXE FALSE)
ELSE(NOT IS_DIRECTORY ${FSO_FS2_PATH})
	SET(FSO_COPY_EXE TRUE)
ENDIF(NOT IS_DIRECTORY ${FSO_FS2_PATH})

OPTION(FSO_CMAKE_DEBUG "Enable cmake debugging in while generating build files" OFF)

MARK_AS_ADVANCED(FORCE FSO_BUILD_PACKAGE)
MARK_AS_ADVANCED(FORCE FSO_CMAKE_DEBUG)
MARK_AS_ADVANCED(FORCE FSO_BUILD_INCLUDED_LIBS)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
INCLUDE(globals)
INCLUDE(toolchain)
INCLUDE(platforms)

INCLUDE(headers)

SET(GENERATED_SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated_source)

# This includes source code for some tools, either used in the build or also for something else
ADD_SUBDIRECTORY(tools)

ADD_SUBDIRECTORY(lib)

ADD_SUBDIRECTORY(code)

ADD_SUBDIRECTORY(freespace2)

IF(FSO_BUILD_FRED2)
	ADD_SUBDIRECTORY(fred2)
ENDIF(FSO_BUILD_FRED2)

IF(FSO_BUILD_WXFRED2)
	ADD_SUBDIRECTORY(wxfred2)
ENDIF(FSO_BUILD_WXFRED2)

IF(FSO_BUILD_PACKAGE)
	include(package)
ENDIF(FSO_BUILD_PACKAGE)
