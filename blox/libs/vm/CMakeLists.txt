file(GLOB SOURCES "vm/*.cc")

find_package(Boost REQUIRED)

add_library(vm ${SOURCES})

target_link_libraries(vm PUBLIC spdlog::spdlog Boost::headers fmt::fmt)

target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


