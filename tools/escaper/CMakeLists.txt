
SET(ESCAPER_SOURCES escaper.cpp)

ADD_EXECUTABLE(escaper ${ESCAPER_SOURCES})

set_target_properties(escaper
	PROPERTIES
		FOLDER "Tools"
)
