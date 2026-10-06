if(WIN32)
    set(BLASTMASTER_SETUP_MANIFEST
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_setup.manifest")
endif()

add_executable(blastmaster_setup
    main.cpp
    SetupWizard.cpp
    SetupWizard.h
    InstallerBackend.cpp
    InstallerBackend.h
    WindowsIntegration.cpp
    WindowsIntegration.h
)

if(WIN32)
    target_sources(blastmaster_setup
        PRIVATE
            "${BLASTMASTER_SETUP_MANIFEST}"
            "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_setup.rc")
    set_source_files_properties(
        "${BLASTMASTER_SETUP_MANIFEST}"
        PROPERTIES
            HEADER_FILE_ONLY TRUE)
    set_target_properties(blastmaster_setup PROPERTIES
        WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_setup
    PRIVATE
        Qt6::Widgets
        Qt6::Core
        blastmaster_common
)

add_executable(blastmaster_uninstaller
    Uninstaller.cpp
    WindowsIntegration.cpp
    WindowsIntegration.h
)

if(WIN32)
    target_sources(blastmaster_uninstaller
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_uninstaller.rc")
    set_target_properties(blastmaster_uninstaller PROPERTIES
        WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_uninstaller
    PRIVATE
        Qt6::Widgets
        Qt6::Core
)

target_include_directories(blastmaster_uninstaller
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/include
)

add_dependencies(blastmaster_setup blastmaster_uninstaller)

target_include_directories(blastmaster_setup
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/include
)

set(BLASTMASTER_SETUP_PAYLOAD
    "$<TARGET_FILE_DIR:blastmaster_setup>/payload"
)

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt windeployqt6)
endif()

add_custom_command(
    TARGET blastmaster_setup
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${BLASTMASTER_SETUP_PAYLOAD}/common"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${BLASTMASTER_SETUP_PAYLOAD}/standard"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:blastmaster_uninstaller>"
        "${BLASTMASTER_SETUP_PAYLOAD}/common/Uninstall$<TARGET_FILE_SUFFIX:blastmaster_uninstaller>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:blastmaster_docs>"
        "${BLASTMASTER_SETUP_PAYLOAD}/standard/blastmaster_docs$<TARGET_FILE_SUFFIX:blastmaster_docs>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:blastmaster_spreadsheet>"
        "${BLASTMASTER_SETUP_PAYLOAD}/standard/blastmaster_workbooks$<TARGET_FILE_SUFFIX:blastmaster_spreadsheet>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:blastmaster_presentation>"
        "${BLASTMASTER_SETUP_PAYLOAD}/standard/blastmaster_presentations$<TARGET_FILE_SUFFIX:blastmaster_presentation>"
    VERBATIM
)

if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(
        TARGET blastmaster_setup
        POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "${BLASTMASTER_SETUP_PAYLOAD}/common"
            "$<TARGET_FILE:blastmaster_uninstaller>"
        VERBATIM
    )
endif()
if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    add_custom_command(
        TARGET blastmaster_setup
        POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "${BLASTMASTER_SETUP_PAYLOAD}/standard"
            "$<TARGET_FILE:blastmaster_docs>"
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "${BLASTMASTER_SETUP_PAYLOAD}/standard"
            "$<TARGET_FILE:blastmaster_spreadsheet>"
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "${BLASTMASTER_SETUP_PAYLOAD}/standard"
            "$<TARGET_FILE:blastmaster_presentation>"
        VERBATIM
    )
endif()

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_custom_command(
        TARGET blastmaster_setup
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${BLASTMASTER_SETUP_PAYLOAD}/professional"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:blastmaster_database>"
            "${BLASTMASTER_SETUP_PAYLOAD}/professional/blastmaster_database$<TARGET_FILE_SUFFIX:blastmaster_database>"
        VERBATIM
    )
endif()

if(WIN32 AND WINDEPLOYQT_EXECUTABLE AND BLASTMASTER_BUILD_PROFESSIONAL)
    add_custom_command(
        TARGET blastmaster_setup
        POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            --no-translations
            --dir "${BLASTMASTER_SETUP_PAYLOAD}/professional"
            "$<TARGET_FILE:blastmaster_database>"
        VERBATIM
    )
endif()
