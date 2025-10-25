file(GLOB SOURCES "*.cc")

add_executable(blox ${SOURCES}) 

target_link_libraries(blox PRIVATE driver)

install(TARGETS blox DESTINATION ${PROJECT_BINARY_DIR})

