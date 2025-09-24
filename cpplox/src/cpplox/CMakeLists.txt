file(GLOB SOURCES "*.cc")

find_package(Boost REQUIRED)

add_library(cpplox ${SOURCES})

target_link_libraries(cpplox PUBLIC spdlog::spdlog magic_enum Boost::headers fmt::fmt)

target_include_directories(cpplox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
