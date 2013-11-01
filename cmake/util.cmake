
FUNCTION(MAKE_CACHE_INTERNAL VARIABLE)
	SET(${VARIABLE} ${${VARIABLE}} CACHE INTERNAL "Internal cache variable")
ENDFUNCTION(MAKE_CACHE_INTERNAL)

FUNCTION(PKG_CONFIG_LIB_RESOLVE NAME OUTVAR)
	foreach(lib ${${NAME}_LIBRARIES})
		find_library(${lib}_LIBRARY
					NAMES ${lib}
					HINTS ${${NAME}_LIBDIR} ${${NAME}_LIBRARY_DIRS}
		)
		
		if (NOT ${${lib}_LIBRARY} MATCHES ".*NOTFOUND.*")
			SET(${OUTVAR} ${OUTVAR} ${${lib}_LIBRARY})
		endif (NOT ${${lib}_LIBRARY} MATCHES ".*NOTFOUND.*") 
	endforeach(lib)
ENDFUNCTION(PKG_CONFIG_LIB_RESOLVE)

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
                SET (${v_lib} "${lib}" CACHE INTERNAL "Library 
providing ${func}") 
                BREAK() 
            ENDIF ("HAVE_${func}_IN_${lib}") 
        ENDFOREACH (lib) 
    ENDIF (NOT ${v_func}) 
ENDMACRO (CMAKE_SEARCH_LIBS) 
# End copy

