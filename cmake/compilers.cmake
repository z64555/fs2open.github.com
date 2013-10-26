
# File to execute compiler specific commands

SET(COMPILER_INCLUDES "" CACHE INTERNAL "Compiler specific includes")
SET(COMPILER_LBRARIES "" CACHE INTERNAL "Compiler specific libraries")

ADD_DEFINITIONS(-DNO_CD_CHECK)

IF (MSVC)
	INCLUDE(msvc)
ELSEIF(CMAKE_COMPILER_IS_GNUCC)
	INCLUDE(gcc)
ELSE(MSVC)
	MESSAGE(STATUS "No special handling for this compiler present, good luck!")
	
	IF (NOT FSO_INSTRUCTION_SET STREQUAL "")
		MESSAGE(WARNING "This compiler does not support a special instruction set!")
	
		SET(FSO_INSTRUCTION_SET "")
	ENDIF(NOT FSO_INSTRUCTION_SET STREQUAL "")
ENDIF(MSVC)
