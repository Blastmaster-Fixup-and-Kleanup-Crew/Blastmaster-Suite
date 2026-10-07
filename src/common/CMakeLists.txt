add_library(blastmaster_common
    src/ProductKeyValidator.cpp
    src/HelpTopicsWindow.cpp
)

target_include_directories(blastmaster_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(blastmaster_common PUBLIC cxx_std_17)

qt_add_resources(blastmaster_common "help_resources"
    PREFIX "/"
    FILES help.qrc
)
