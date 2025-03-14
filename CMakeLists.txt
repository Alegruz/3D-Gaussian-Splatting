cmake_minimum_required(VERSION 3.10.0)

project(3D-Gaussian-Splatting VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h src/*.hpp)

if (NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory!")
endif()

add_executable(3D-Gaussian-Splatting ${SOURCES})

target_include_directories(3D-Gaussian-Splatting PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_sources(3D-Gaussian-Splatting PRIVATE ${SOURCES})

if (MSVC)
    target_compile_options(3D-Gaussian-Splatting PRIVATE /W4 /WX)
else()
    target_compile_options(3D-Gaussian-Splatting PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
