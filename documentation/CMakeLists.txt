project(documentation)
include(FindPkgConfig)
find_package(Doxygen)

if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/doxy.config.in
        ${CMAKE_CURRENT_BINARY_DIR}/doxy.config @ONLY
    )

    add_custom_target(documentation
        ${DOXYGEN_EXECUTABLE}
        ${CMAKE_CURRENT_BINARY_DIR}/doxy.config
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif(DOXYGEN_FOUND)
