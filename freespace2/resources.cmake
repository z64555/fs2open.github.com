
if(WIN32)
    # Handling of windows resources+

    set(subpath res/win)

    set(RESOURCE_FILES
        ${subpath}/freespace.rc
    )

    set(ICONS
        ${subpath}/app_icon.ico
        ${subpath}/app_icon_glow.ico
        ${subpath}/dbg_icon.ico
        ${subpath}/goal_com.bmp
        ${subpath}/goal_fail.bmp
        ${subpath}/goal_inc.bmp
        ${subpath}/goal_none.bmp
        ${subpath}/goal_ord.bmp
        ${subpath}/V_app.ico
        ${subpath}/V_debug.ico
        ${subpath}/V_sse-d.ico
        ${subpath}/V_sse.ico
    )

    set(RESOURCES
        ${RESOURCE_FILES}
        ${ICONS}
    )

    source_group("Resources" FILES ${RESOURCE_FILES})
    source_group("Resources\\Icons" FILES ${ICONS})

    SET_SOURCE_FILES_PROPERTIES(${subpath}/freespace.rc PROPERTIES COMPILE_DEFINITIONS "_VC08")

    IF(FSO_INSTRUCTION_SET STREQUAL "SSE2" OR FSO_INSTRUCTION_SET STREQUAL "AVX")
    	SET_SOURCE_FILES_PROPERTIES(${subpath}/freespace.rc PROPERTIES COMPILE_DEFINITIONS "_VC08;_SSE2")
    ENDIF()
elseif(APPLE)
    # Handling of apple resources
    set(subpath res/mac)

    set(RESOURCES
        ${subpath}/FS2_Open.icns
        ${subpath}/English.lproj/InfoPlist.strings
    )

    set_source_files_properties(${subpath}/FS2_Open.icns MACOSX_PACKAGE_LOCATION Resources)
    set_source_files_properties(${subpath}/English.lproj/InfoPlist.strings MACOSX_PACKAGE_LOCATION Resources/English.lproj)

    source_group("Resources" FILES ${RESOURCES})

    set_target_properties(Freespace2 PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}/Info.plist)

    # Also handle copying frameworks here
    file(GLOB FRAMEWORK_FILES
        RELATIVE ${FSO_MAC_FRAMEWORKS}
        ${FSO_MAC_FRAMEWORKS}/*.framework
    )

    foreach(framework ${FRAMEWORK_FILES})
        add_custom_command(TARGET FreeSpace2 POST_BUILD
            COMMENT "Copying ${framework} into bundle..."
            ${CMAKE_COMMAND} -E copy_if_different ${FSO_MAC_FRAMEWORKS}/${framework} $<TARGET_FILE_DIR:${TARGET}>/Contents/Frameworks/${framework}
        )
    endforeach(framework)
else()
    # No special resource handling required, add rules for new platforms here
endif()

target_sources(Freespace2 PRIVATE ${RESOURCES})
