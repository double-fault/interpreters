cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(cpplox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

set(CMAKE_CXX_FLAGS "-g")

add_subdirectory(spdlog)
add_subdirectory(fmt)
add_subdirectory(magic_enum)
add_subdirectory(libs/cpplox)
add_subdirectory(googletest)
add_subdirectory(tests)
add_subdirectory(lox)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)
