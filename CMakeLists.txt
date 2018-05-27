project(pulseviz)
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -pedantic")

find_library(LIBPULSE_SIMPLE pulse-simple)
find_library(LIBPULSE pulse)
find_package(OpenGL REQUIRED)
find_library(glfw glfw)
find_library(GLEW NAMES GLEW)
find_library(FFTW3F fftw3f)
find_package(Threads REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE SHADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.glsl)

add_executable(pulseviz ${SOURCES} ${SHADER_SOURCES})
target_link_libraries(
    pulseviz
    Threads::Threads
    ${LIBPULSE}
    ${LIBPULSE_SIMPLE}
    ${OPENGL_gl_LIBRARY}
    ${glfw}
    ${GLEW}
    ${FFTW3F}
)
add_custom_target(
    copy_shader_files ALL
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shader
        ${CMAKE_BINARY_DIR}/shader
)
add_dependencies(pulseviz copy_shader_files)

add_subdirectory(tests)
