add_executable(blastmaster_spreadsheet
    main.cpp
    Workbook.cpp
    Excel7MenuBar.cpp
)

target_link_libraries(blastmaster_spreadsheet
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    main.cpp
    Workbook.cpp
    Workbook.h
    Excel7MenuBar.cpp
    Excel7MenuBar.h
)
