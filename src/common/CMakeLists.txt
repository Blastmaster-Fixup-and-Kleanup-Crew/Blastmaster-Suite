set(CMAKE_AUTORCC ON)

add_library(blastmaster_common
    src/ProductKeyValidator.cpp
    src/HelpTopicsWindow.cpp
)

target_include_directories(blastmaster_common
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(blastmaster_common PUBLIC cxx_std_17)

target_sources(blastmaster_common PRIVATE help.qrc)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(blastmaster_common PUBLIC Qt6::Widgets)
