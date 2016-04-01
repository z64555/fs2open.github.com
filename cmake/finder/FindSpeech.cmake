
add_library(speech INTERFACE)

if (WIN32)
	find_package(WindowsSDK REQUIRED)
	
	get_windowssdk_from_component(sapi.lib WIN_SDK)
	
	get_windowssdk_library_dirs("${WIN_SDK}" LIB_DIRS)
	get_windowssdk_include_dirs("${WIN_SDK}" INCLUDE_DIRS)
	
	find_library(SPEECH_LIBRARY
		NAMES sapi
		PATHS ${LIB_DIRS}
		NO_DEFAULT_PATH)
		
	if (NOT SPEECH_LIBRARY)
		message(SEND_ERROR "Text to speech library could not be found! Either install it or disable the speech option.")
	endif()
	
	target_link_libraries(speech INTERFACE ${SPEECH_LIBRARY})
	target_include_directories(speech INTERFACE ${INCLUDE_DIRS})
else()
	message(SEND_ERROR "Text to Speech is not supported on this platform!")
endif()

target_compile_definitions(speech INTERFACE -DFS2_SPEECH)