
SET(MONGOOSE_SOURCES
	mongoose.c
	mongoose.h)

ADD_LIBRARY(mongoose ${MONGOOSE_SOURCES})

set_target_properties(mongoose
	PROPERTIES
		FOLDER "3rdparty"
)

IF(MSVC)
	set_target_properties(mongoose
		PROPERTIES
			STATIC_LIBRARY_FLAGS "/LTCG"
	)
ENDIF(MSVC)

SET(MONGOOSE_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "mongoose include directory")

SET(MONGOOSE_LIBRARIES mongoose CACHE INTERNAL "mongoose library")
