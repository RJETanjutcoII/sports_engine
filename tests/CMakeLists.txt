# Tests CMakeLists.txt

# For now, just a placeholder
# We'll add unit tests for physics once that system is built

add_executable(SportsEngineTests
    placeholder_test.cpp
)

target_link_libraries(SportsEngineTests PRIVATE
    GTest::gtest_main
    glm::glm
)

include(GoogleTest)
gtest_discover_tests(SportsEngineTests)
