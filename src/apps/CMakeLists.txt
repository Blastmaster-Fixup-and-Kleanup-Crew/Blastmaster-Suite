# Resolve deployment tools before configuring app subdirectories, because their
# POST_BUILD commands use these variables.
if(WIN32)
    find_program(BLASTMASTER_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${Qt6_DIR}/../../../bin"
    )
    if(NOT BLASTMASTER_WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt was not found. Add the Qt bin directory to PATH so Windows app builds can deploy their Qt runtime DLLs.")
    endif()

    find_program(POWERSHELL_EXECUTABLE NAMES powershell pwsh)
    if(NOT POWERSHELL_EXECUTABLE)
        message(FATAL_ERROR "PowerShell is required to deploy the Windows app runtime DLLs.")
    endif()
endif()

add_subdirectory(docs)
add_subdirectory(presentation)
add_subdirectory(spreadsheet)

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_subdirectory(database)
endif()
