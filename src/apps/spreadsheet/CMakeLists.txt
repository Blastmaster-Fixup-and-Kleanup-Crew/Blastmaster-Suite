add_executable(blastmaster_spreadsheet
    main.cpp
    Workbook.cpp
    Excel7MenuBar.cpp
)

if(WIN32)
    target_sources(blastmaster_spreadsheet PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_workbooks.rc")
    set_target_properties(blastmaster_spreadsheet PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_spreadsheet
    PRIVATE Qt6::Widgets blastmaster_common)

if(WIN32)
    add_custom_command(TARGET blastmaster_spreadsheet POST_BUILD
        COMMAND "${BLASTMASTER_WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                "$<TARGET_FILE:blastmaster_spreadsheet>"
        COMMENT "Deploying Qt runtime for blastmaster_spreadsheet"
        VERBATIM
    )
endif()
