add_subdirectory(word)
add_subdirectory(presentation)
add_subdirectory(spreadsheet)

if(BLASTMASTER_BUILD_PROFESSIONAL)
    add_subdirectory(database)
endif()
