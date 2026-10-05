add_executable(blastmaster_database
    main.cpp
)

target_link_libraries(blastmaster_database
    PRIVATE
        Qt6::Widgets
        Qt6::Sql
        blastmaster_common
)
