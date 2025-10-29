cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

project(blox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

# TODO: Add a debug mode which has address sanitizer and debug flags, and release with -O3 -NDEBUG
# (Nah add address sanitizer after implementing GC or else it detects memory leaks :clown:)
set(CMAKE_CXX_FLAGS "-O3")

add_subdirectory(fmt)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(SPDLOG_FMT_EXTERNAL ON)
add_subdirectory(spdlog)
add_subdirectory(googletest)
add_subdirectory(magic_enum)

add_subdirectory(libs/ir)
add_subdirectory(libs/compiler)
add_subdirectory(libs/vm)
add_subdirectory(libs/driver)
add_subdirectory(blox)

add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

