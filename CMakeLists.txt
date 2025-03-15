cmake_minimum_required(VERSION 3.10.0)

project(3D-Gaussian-Splatting VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(
    GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp 
    ${PROJECT_SOURCE_DIR}/src/*.h 
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/external/spz/src/cc/*.cc
    ${PROJECT_SOURCE_DIR}/external/zlib/*.c
    )

if (NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory!")
endif()

add_executable(3D-Gaussian-Splatting ${SOURCES})

target_include_directories(
    3D-Gaussian-Splatting PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/spz/src/cc
    ${PROJECT_SOURCE_DIR}/external/zlib
    )

target_sources(3D-Gaussian-Splatting PRIVATE ${SOURCES})

target_compile_definitions(3D-Gaussian-Splatting PRIVATE _USE_MATH_DEFINES)
if (MSVC)
    target_compile_options(3D-Gaussian-Splatting PRIVATE /W4 /WX)
else()
    target_compile_options(3D-Gaussian-Splatting PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
