
SET(EMBEDFILE_SOURCES embedfile.cpp)

ADD_EXECUTABLE(embedfile ${EMBEDFILE_SOURCES})

set_target_properties(embedfile
	PROPERTIES
		FOLDER "Tools"
)
