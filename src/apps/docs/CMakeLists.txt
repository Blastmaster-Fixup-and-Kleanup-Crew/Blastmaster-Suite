add_executable(blastmaster_docs
    main.cpp
    Document.cpp
    Word7MenuBar.cpp
    icons/icons.qrc
)

target_link_libraries(blastmaster_docs
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    main.cpp
    Document.cpp
    Document.h
    Word7MenuBar.cpp
    Word7MenuBar.h
    icons/icons.qrc
)
