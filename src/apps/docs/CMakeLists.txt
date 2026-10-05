add_executable(blastmaster_docs
    main.cpp
)

target_link_libraries(blastmaster_docs
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)
