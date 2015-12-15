cmake_minimum_required(VERSION 3.2)
project(graphix_editor_main)

enable_testing()

add_subdirectory(documentation)
add_subdirectory(graphix-editor)
add_subdirectory(unit-testing)

add_dependencies(documentation graphix_editor)
add_dependencies(unit_testing graphix_editor documentation)
