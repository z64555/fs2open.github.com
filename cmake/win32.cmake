
SET(PLATFORM_INCLUDES "" CACHE INTERNAL "Compiler specific includes")
SET(PLATFORM_LBRARIES "" CACHE INTERNAL "Compiler specific libraries") 

SET(PLATFORM_LBRARIES winmm.lib wsock32.lib comctl32.lib) 
	
# DirectX libraries are still used on windows
SET(DIRECTX_DIRECTORY ${CMAKE_SOURCE_DIR}/code/directx)
SET(PLATFORM_LBRARIES winmm.lib wsock32.lib comctl32.lib) 
SET(PLATFORM_LBRARIES ${DIRECTX_DIRECTORY}/dxguid.lib)
SET(PLATFORM_LBRARIES ${DIRECTX_DIRECTORY}/strmiids.lib)
SET(PLATFORM_LBRARIES ${DIRECTX_DIRECTORY}/vdinput.lib) 
