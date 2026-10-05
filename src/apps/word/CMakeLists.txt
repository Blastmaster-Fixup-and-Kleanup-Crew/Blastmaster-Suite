add_executable(blastmaster_word
    main.cpp
)

target_link_libraries(blastmaster_word
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)
