
FUNCTION(MAKE_CACHE_INTERNAL VARIABLE)
	SET(${VARIABLE} ${${VARIABLE}} CACHE INTERNAL "Internal cache variable")
ENDFUNCTION(MAKE_CACHE_INTERNAL)

FUNCTION(PKG_CONFIG_LIB_RESOLVE NAME)
	foreach(lib ${${NAME}_LIBRARIES}) 
		find_package(${lib}_LIBRARY 
					NAMES ${lib}
					HINTS ${${NAME}_LIBRARY_DIRS}
		)
		
		if (${${lib}_LIBRARY})
			MESSAGE("$[lib}: ${${lib}_LIBRARY}")
			#list(APPEND ${${NAME}_LIBRARIES} ${${lib}_LIBRARY}) 
		endif (${${lib}_LIBRARY}) 
	endforeach(lib)
ENDFUNCTION(PKG_CONFIG_LIB_RESOLVE)
