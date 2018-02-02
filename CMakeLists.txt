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

file(GLOB SOURCES src/*.cpp src/**/*.cpp src/**/**/*.cpp)
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

