enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

file(GLOB TEST_SOURCES
    ../src/dsp/*.cpp
    ../src/gfx/*.cpp
    ../src/pulseaudio.cpp
    ../src/iniparser.cpp
    ../src/config.cpp
    ../src/util.cpp
    *.cpp
)
add_executable(run_tests ${TEST_SOURCES})
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
