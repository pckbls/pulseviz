enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

# Allows accessing the test data directory from within the C++ test code.
# Thanks: https://cmake.org/pipermail/cmake/2009-November/033047.html
add_definitions(-DTEST_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DSHARE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../share")

set(SOURCES_WITHOUT_MAIN ${SOURCES})
list(FILTER SOURCES_WITHOUT_MAIN EXCLUDE REGEX ".*\/main.cpp$")

file(GLOB TEST_SOURCES
    ${SOURCES_WITHOUT_MAIN}
    *.cpp
)
file(GLOB TEST_DATA
    ./data/*.ini
    ../share/*.ini
)

add_executable(run_tests
    EXCLUDE_FROM_ALL
    ${TEST_SOURCES}
    ${TEST_DATA}
)
target_include_directories(
    run_tests PUBLIC
    ${GLEW_INCLUDE_DIRS}
    ${LIBPULSE_INCLUDE_DIRS}
    ${LIBPULSE_SIMPLE_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
)
target_link_libraries(run_tests
    Threads::Threads
    glfw
    ${LIBPULSE_LIBRARIES}
    ${LIBPULSE_SIMPLE_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FFTW3F_LIBRARIES}
    GTest::GTest
    GTest::Main
)
add_custom_target(test)
add_dependencies(test run_tests)

gtest_discover_tests(run_tests)
