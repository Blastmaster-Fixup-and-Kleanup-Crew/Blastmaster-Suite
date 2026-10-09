add_subdirectory(docs)
add_subdirectory(presentation)
add_subdirectory(spreadsheet)

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_subdirectory(database)
endif()

# A Windows build must be runnable from its build directory, not just on a
# machine that happens to have Qt on PATH. windeployqt copies the Qt runtime
# DLLs and required platform plugins beside each executable after linking.
if(WIN32)
    find_program(BLASTMASTER_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${Qt6_DIR}/../../../bin"
    )
    if(NOT BLASTMASTER_WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt was not found. Add the Qt bin directory to PATH so Windows app builds can deploy their Qt runtime DLLs.")
    endif()
    set(BLASTMASTER_APP_TARGETS
        blastmaster_docs
        blastmaster_presentation
        blastmaster_spreadsheet
    )
    if(BLASTMASTER_BUILD_PROFESSIONAL)
        list(APPEND BLASTMASTER_APP_TARGETS blastmaster_database)
    endif()

    foreach(app_target IN LISTS BLASTMASTER_APP_TARGETS)
        add_custom_command(TARGET ${app_target} POST_BUILD
            COMMAND "${BLASTMASTER_WINDEPLOYQT_EXECUTABLE}"
                    --no-translations
                    "$<TARGET_FILE:${app_target}>"
            COMMENT "Deploying Qt runtime for ${app_target}"
            VERBATIM
        )
    endforeach()
endif()
