file(GLOB SOURCES "*.cc")

add_executable(lox ${SOURCES}) 

target_link_libraries(lox PRIVATE cpplox)

install(TARGETS lox DESTINATION ${PROJECT_BINARY_DIR})
