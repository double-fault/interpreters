enable_testing()

add_executable(scanner_test
    scanner_test.cc
)

target_link_libraries(scanner_test
    gtest
    gtest_main
    gmock
    gmock_main
    cpplox
)

include(GoogleTest)
gtest_discover_tests(scanner_test)
