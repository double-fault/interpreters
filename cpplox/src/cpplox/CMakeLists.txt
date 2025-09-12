file(GLOB SOURCES "*.cc")

add_library(cpplox ${SOURCES})

target_link_libraries(cpplox PUBLIC spdlog::spdlog magic_enum)

target_include_directories(cpplox PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
