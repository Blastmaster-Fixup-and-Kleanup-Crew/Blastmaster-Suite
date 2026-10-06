add_subdirectory(docs)
add_subdirectory(presentation)
add_subdirectory(spreadsheet)

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_subdirectory(database)
endif()
