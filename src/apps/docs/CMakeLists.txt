add_executable(blastmaster_docs
    main.cpp
    Document.cpp
    Word7MenuBar.cpp
    icons/icons.qrc
)

if(WIN32)
    target_sources(blastmaster_docs PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/blastmaster_docs.rc")
    set_target_properties(blastmaster_docs PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(blastmaster_docs
    PRIVATE Qt6::Widgets blastmaster_common)

if(WIN32)
    add_custom_command(TARGET blastmaster_docs POST_BUILD
        COMMAND "${BLASTMASTER_WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                "$<TARGET_FILE:blastmaster_docs>"
        COMMENT "Deploying Qt runtime for blastmaster_docs"
        VERBATIM
    )
endif()
