file(GLOB SOURCES "driver/*.cc")

find_package(Boost REQUIRED)

add_library(driver ${SOURCES})

target_link_libraries(driver PUBLIC spdlog::spdlog Boost::headers fmt::fmt 
    ir compiler vm)

target_include_directories(driver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

