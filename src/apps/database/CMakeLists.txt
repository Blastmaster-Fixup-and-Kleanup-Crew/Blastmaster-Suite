add_executable(blastmaster_database
    main.cpp
    Access95MenuBar.cpp
    DatabaseDocument.cpp
    DatabaseDocument.h
)

if(WIN32)
    target_sources(blastmaster_database PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_database.rc")
    set_target_properties(blastmaster_database PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_database
    PRIVATE Qt6::Widgets Qt6::Sql blastmaster_common)
