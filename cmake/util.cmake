
FUNCTION(MAKE_CACHE_INTERNAL VARIABLE)
	SET(${VARIABLE} ${${VARIABLE}} CACHE INTERNAL "Internal cache variable")
ENDFUNCTION(MAKE_CACHE_INTERNAL)

MACRO(PKG_CONFIG_LIB_RESOLVE NAME OUTVAR)
	SET(${OUTVAR} "" CACHE INTERNAL "" FORCE)
	foreach(lib ${${NAME}_LIBRARIES})
		find_library(${lib}_LIBRARY
					NAMES ${lib}
					HINTS ${${NAME}_LIBDIR} ${${NAME}_LIBRARY_DIRS}
		)
	
		if (NOT ${${lib}_LIBRARY} MATCHES ".*NOTFOUND.*")
			SET(${OUTVAR} ${${OUTVAR}} ${${lib}_LIBRARY})
		endif (NOT ${${lib}_LIBRARY} MATCHES ".*NOTFOUND.*") 
	endforeach(lib)
ENDMACRO(PKG_CONFIG_LIB_RESOLVE)

# Copy from http://cmake.3232098.n2.nabble.com/RFC-cmake-analog-to-AC-SEARCH-LIBS-td7585423.html
INCLUDE (CheckFunctionExists) 
INCLUDE (CheckLibraryExists) 

MACRO (CMAKE_SEARCH_LIBS v_func v_lib func) 
    CHECK_FUNCTION_EXISTS (${func} ${v_func}) 
    IF (NOT ${v_func}) 
        FOREACH (lib ${ARGN}) 
            CHECK_LIBRARY_EXISTS (${lib} ${func} "" "HAVE_${func}_IN_${lib}") 
            IF ("HAVE_${func}_IN_${lib}") 
                SET (${v_func} TRUE) 
                SET (${v_lib} "${lib}" CACHE INTERNAL "Library providing ${func}") 
                BREAK() 
            ENDIF ("HAVE_${func}_IN_${lib}") 
        ENDFOREACH (lib) 
    ENDIF (NOT ${v_func}) 
ENDMACRO (CMAKE_SEARCH_LIBS) 
# End copy

FUNCTION(EP_CHECK_FILE_EXISTS FILE OUTVAR TARGET NAME COMMAND WORKING_DIR)
	string (REPLACE ";" " " COMMAND_STR "${COMMAND}")
	
	FILE(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${NAME}" "
IF(EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${FILE}\")
	MESSAGE(\"'${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${FILE}' already exists, nothing to be done.\")
ELSE(EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${FILE}\")
	execute_process(COMMAND cmd /C \"${COMMAND_STR}\" WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${WORKING_DIR}\")
ENDIF(EXISTS \"${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${FILE}\")
")
	
	SET(${OUTVAR} "${CMAKE_COMMAND}" -P ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/${NAME} PARENT_SCOPE)
ENDFUNCTION(EP_CHECK_FILE_EXISTS)
