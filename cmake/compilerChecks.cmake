
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)
include(CheckPrototypeDefinition)
include(CheckStructHasMember)
include(CheckSymbolExists)
include(CheckTypeSize)

CONFIGURE_FILE(${CMAKE_CURRENT_LIST_DIR}/compilerChecks.h.in ${GENERATED_SOURCE_DIR}/compilerChecks.h)
