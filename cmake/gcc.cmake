
MESSAGE(STATUS "Doing configuration specific to gcc...")

INCLUDE(util)

ADD_COMPILER_FLAG("-Wno-unused-parameter -Wno-write-strings" "-Wno-unused-parameter -Wno-write-strings")

