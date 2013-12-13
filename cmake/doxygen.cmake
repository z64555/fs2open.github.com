
# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
	add_custom_target(doxygen
		${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/fs2open.Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM
	)
	
	set_target_properties(doxygen
		PROPERTIES
			FOLDER "Documentation"
			EXCLUDE_FROM_ALL ON
			EXCLUDE_FROM_DEFAULT_BUILD ON
	)
endif(DOXYGEN_FOUND)
