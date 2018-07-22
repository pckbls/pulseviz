project(pulseviz)
cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor -pedantic")

find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLEW REQUIRED glew)
pkg_search_module(LIBPULSE REQUIRED libpulse)
pkg_search_module(LIBPULSE_SIMPLE REQUIRED libpulse-simple)
pkg_search_module(FFTW3F REQUIRED fftw3f)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE SHADER_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.glsl)
file(GLOB_RECURSE SHARED_FILES share/*.*)

add_executable(pulseviz ${SOURCES} ${SHADER_SOURCES} ${SHARED_FILES})
target_include_directories(
    pulseviz PUBLIC
    ${GLEW_INCLUDE_DIRS}
    ${LIBPULSE_INCLUDE_DIRS}
    ${LIBPULSE_SIMPLE_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
)
target_link_libraries(
    pulseviz
    Threads::Threads
    glfw
    ${LIBPULSE_LIBRARIES}
    ${LIBPULSE_SIMPLE_LIBRARIES}
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)
add_custom_target(
    copy_shader_files ALL
    COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shader
        ${CMAKE_BINARY_DIR}/shader
)
add_custom_target(
    copy_default_config ALL
    COMMAND ${CMAKE_COMMAND}
        -E copy
        ${CMAKE_SOURCE_DIR}/share/pulseviz.ini
        ${CMAKE_BINARY_DIR}/pulseviz.ini
)
add_dependencies(pulseviz copy_shader_files copy_default_config)

add_subdirectory(tests)
