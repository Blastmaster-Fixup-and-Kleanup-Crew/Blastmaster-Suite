add_executable(blastmaster_presentation
    main.cpp
    PresentationDocument.cpp
    PowerPoint7MenuBar.cpp
    icons/icons.qrc
)

if(WIN32)
    target_sources(blastmaster_presentation PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_presentations.rc")
    set_target_properties(blastmaster_presentation PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_presentation
    PRIVATE Qt6::Widgets blastmaster_common)
