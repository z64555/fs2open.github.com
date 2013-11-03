
SET(EMBEDPNG_SOURCES embedpng.cpp)

ADD_EXECUTABLE(embedpng ${EMBEDPNG_SOURCES})

set_target_properties(embedpng
	PROPERTIES
		FOLDER "Tools"
)
