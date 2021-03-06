project(graphix_editor)
include(FindPkgConfig)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GRAPHIX REQUIRED "graphix >= 0.0.1")

add_definitions(
    "-std=gnu++11"
    "-Wall"
    "-W"
    "-pedantic"
    "-Wextra"
    "-Wconversion"
    "-Winit-self"
    "-Woverloaded-virtual"
    "-Wunreachable-code"
)

add_executable(graphix_editor
    src/graphix-editor.cpp
    src/wasd.cpp
    src/wasd.hpp
)

set_target_properties(graphix_editor
    PROPERTIES OUTPUT_NAME graphix-editor
)

target_include_directories(graphix_editor
    PRIVATE
        ${GRAPHIX_INCLUDE_DIRS}
        src
)

target_link_libraries(graphix_editor
    ${GRAPHIX_LIBRARIES}
)

install(TARGETS graphix_editor
    RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin"
)