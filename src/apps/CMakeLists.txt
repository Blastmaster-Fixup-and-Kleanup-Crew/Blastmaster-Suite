add_subdirectory(docs)
add_subdirectory(presentation)
add_subdirectory(spreadsheet)

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_subdirectory(database)
endif()

# A Windows build must be runnable from its build directory, not just on a
# machine that happens to have Qt on PATH. Each app's CMakeLists.txt attaches
# windeployqt as a POST_BUILD command in the same directory where its target
# is created; CMake does not allow TARGET post-build commands from this parent
# directory for targets created inside child directories.
if(WIN32)
    find_program(BLASTMASTER_WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6
        HINTS "${Qt6_DIR}/../../../bin"
    )
    if(NOT BLASTMASTER_WINDEPLOYQT_EXECUTABLE)
        message(FATAL_ERROR "windeployqt was not found. Add the Qt bin directory to PATH so Windows app builds can deploy their Qt runtime DLLs.")
    endif()
endif()
