enable_testing()

configure_file(test.mov ${CMAKE_CURRENT_BINARY_DIR}/test.mov COPYONLY)

add_executable(qtff_test qtff_test.cpp)
target_link_libraries(qtff_test qtff GTest::gtest_main)

gtest_discover_tests(qtff_test)

add_custom_target(
    check
    ALL
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS qtff_test
)
