add_executable(blastmaster_presentation
    main.cpp
)

target_link_libraries(blastmaster_presentation
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)
