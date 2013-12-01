
IF(NOT CMAKE_CROSSCOMPILING) # The Cmake documentation says we should do this
	TRY_RUN(RUN_RESULT COMPILE_RESULT "${CMAKE_BINARY_DIR}/temp" "${CMAKE_SOURCE_DIR}/cmake/cpufeatures.cpp"
		RUN_OUTPUT_VARIABLE FEATURE_OUTPUT)
	
	IF(COMPILE_RESULT)
		MESSAGE(STATUS "Detected compatibility for the ${FEATURE_OUTPUT} feature set. Optimizations are done accordingly.")
	
		SET(FSO_INSTRUCTION_SET ${FEATURE_OUTPUT} CACHE STRING "The instruction set enhancement to use, possible options are SSE, SSE2 and AVX")
	ELSE(COMPILE_RESULT)
		MESSAGE("Compilation of CPU feature detector failed, please set the instructionset manually.")
	ENDIF(COMPILE_RESULT)
ENDIF(NOT CMAKE_CROSSCOMPILING)
