add_executable(blastmaster_presentation
    main.cpp
    PresentationDocument.cpp
    PowerPoint7MenuBar.cpp
    icons/icons.qrc
)

target_link_libraries(blastmaster_presentation
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    main.cpp
    PresentationDocument.cpp
    PresentationDocument.h
    PowerPoint7MenuBar.cpp
    PowerPoint7MenuBar.h
    icons/icons.qrc
)
