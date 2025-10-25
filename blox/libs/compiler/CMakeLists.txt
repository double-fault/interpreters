file(GLOB SOURCES "compiler/*.cc")

find_package(Boost REQUIRED)

add_library(compiler ${SOURCES})

target_link_libraries(compiler PUBLIC spdlog::spdlog Boost::headers fmt::fmt magic_enum ir)

target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

