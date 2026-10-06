add_executable(blastmaster_setup
    main.cpp
    SetupWizard.cpp
    SetupWizard.h
)

target_link_libraries(blastmaster_setup
    PRIVATE
        Qt6::Widgets
        blastmaster_common
)

target_include_directories(blastmaster_setup
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/common/include
)
