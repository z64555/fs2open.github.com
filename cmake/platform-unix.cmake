
INCLUDE(util)

MESSAGE(STATUS "Configuring UNIX specific things and stuff...")

target_compile_definitions(platform INTERFACE SCP_UNIX USE_OPENAL)

FIND_PACKAGE(OpenGL REQUIRED)

target_include_directories(platform INTERFACE ${PLATFORM_INCLUDES} ${OPENGL_INCLUDE_DIR})
target_link_libraries(platform INTERFACE ${PLATFORM_LBRARIES} ${OPENGL_LIBRARY})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	include(platform-darwin)
ELSE(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	SET(EXE_GUI_TYPE)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
