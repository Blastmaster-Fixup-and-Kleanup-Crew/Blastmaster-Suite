add_executable(blastmaster_database
    main.cpp
    Access95MenuBar.cpp
    DatabaseDocument.cpp
)

target_link_libraries(blastmaster_database
    PRIVATE
        Qt6::Widgets
        Qt6::Sql
        blastmaster_common
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    main.cpp
    DatabaseDocument.cpp
    DatabaseDocument.h
    Access95MenuBar.cpp
    Access95MenuBar.h
)
