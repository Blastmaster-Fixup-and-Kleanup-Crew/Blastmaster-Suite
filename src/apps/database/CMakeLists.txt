add_executable(blastmaster_database
    main.cpp
    database_icons.qrc
    Access95MenuBar.cpp
    DatabaseDocument.cpp
    DatabaseDocument.h
    ${CMAKE_SOURCE_DIR}/src/common/office95_toolbar_icons.qrc
)
if(WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_database.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/blastmaster_database.rc" @ONLY)
    target_sources(blastmaster_database PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/blastmaster_database.rc")
    set_target_properties(blastmaster_database PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
target_link_libraries(blastmaster_database PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport blastmaster_common)
if(WIN32)
    add_custom_command(TARGET blastmaster_database POST_BUILD
        COMMAND "${POWERSHELL_EXECUTABLE}" -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/../DeployAppRuntime.ps1"
            -AppExe "$<TARGET_FILE:blastmaster_database>"
            -WinDeployQt "${BLASTMASTER_WINDEPLOYQT_EXECUTABLE}"
        COMMENT "Deploying Qt and compiler runtime for blastmaster_database" VERBATIM)
endif()
