add_executable(blastmaster_spreadsheet
    main.cpp
)

target_link_libraries(blastmaster_spreadsheet
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)
