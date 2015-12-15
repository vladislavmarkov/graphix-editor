project(unit_testing)
include(FindPkgConfig)
find_package(PkgConfig REQUIRED)
find_package(GTest REQUIRED)

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

add_executable(unit_testing
    src/unit_testing.cpp
)

target_include_directories(unit_testing
    PUBLIC graphix ${GTEST_INCLUDE_DIRS}
    PRIVATE src
)

link_directories(graphix)

target_link_libraries(unit_testing
    graphix
    ${GTEST_BOTH_LIBRARIES}
    pthread
)

add_test(
    NAME unit_testing
    COMMAND unit_testing
)
