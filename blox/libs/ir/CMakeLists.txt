file(GLOB SOURCES "ir/*.cc")

find_package(Boost REQUIRED)

add_library(ir ${SOURCES})

target_link_libraries(ir PUBLIC spdlog::spdlog Boost::headers fmt::fmt magic_enum)

target_include_directories(ir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


