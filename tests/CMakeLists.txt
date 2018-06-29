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

add_executable(run_tests ${TEST_SOURCES} ${TEST_DATA})
target_link_libraries(run_tests
    Threads::Threads
    ${LIBPULSE}
    ${LIBPULSE_SIMPLE}
    ${OPENGL_gl_LIBRARY}
    ${glfw}
    ${GLEW}
    ${FFTW3F}
    GTest::GTest
    GTest::Main
)

gtest_add_tests(TARGET run_tests)
