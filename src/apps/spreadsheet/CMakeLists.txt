add_executable(blastmaster_workbook
    main.cpp
    Workbook.cpp
    Excel7MenuBar.cpp
)

if(WIN32)
    target_sources(blastmaster_workbook PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_workbooks.rc")
    set_target_properties(blastmaster_workbook PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_workbook
    PRIVATE Qt6::Widgets blastmaster_common)

if(WIN32)
    add_custom_command(TARGET blastmaster_workbook POST_BUILD
        COMMAND "${BLASTMASTER_WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                "$<TARGET_FILE:blastmaster_workbook>"
        COMMENT "Deploying Qt runtime for blastmaster_workbook"
        VERBATIM
    )
endif()
