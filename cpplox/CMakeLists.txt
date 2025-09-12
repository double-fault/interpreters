cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(spdlog)
add_subdirectory(magic_enum)
add_subdirectory(src/cpplox)
add_subdirectory(lox)
