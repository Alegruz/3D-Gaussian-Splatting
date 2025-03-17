cmake_minimum_required(VERSION 3.10.0)

project(3D-Gaussian-Splatting VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(external/zlib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(
    GLOB_RECURSE SOURCES 
    ${PROJECT_SOURCE_DIR}/src/*.cpp 
    ${PROJECT_SOURCE_DIR}/src/*.h 
    ${PROJECT_SOURCE_DIR}/src/*.hpp
    ${PROJECT_SOURCE_DIR}/3dgs/graphics/*.hpp
    )
file(GLOB_RECURSE SPZ_SOURCES ${PROJECT_SOURCE_DIR}/external/spz/src/cc/*.cc)

if (NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory!")
endif()

add_executable(3D-Gaussian-Splatting ${SOURCES} ${SPZ_SOURCES})

target_include_directories(
    3D-Gaussian-Splatting PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/external/spz/src/cc
    ${PROJECT_SOURCE_DIR}/external/zlib
    ${PROJECT_SOURCE_DIR}/external/slang/include
    )

target_sources(3D-Gaussian-Splatting PRIVATE ${SOURCES})

# Add precompiled header
target_sources(3D-Gaussian-Splatting PRIVATE ${PROJECT_SOURCE_DIR}/src/pch.cpp)
target_precompile_headers(3D-Gaussian-Splatting PRIVATE ${PROJECT_SOURCE_DIR}/include/pch.h)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(3D-Gaussian-Splatting PRIVATE DEBUG)
    add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${PROJECT_SOURCE_DIR}/build/external/zlib/Debug/zlibd.dll
        $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
    )
else()
target_compile_definitions(3D-Gaussian-Splatting PRIVATE RELEASE)
add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/build/external/zlib/Release/zlib.dll
    $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
)
endif()

add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/external/slang/bin/slang-rt.dll
    $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
)

add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/external/slang/bin/slang.dll
    $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
)

add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/external/slang/bin/gfx.dll
    $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
)

add_custom_command(TARGET 3D-Gaussian-Splatting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/external/slang/bin/slang-llvm.dll
    $<TARGET_FILE_DIR:3D-Gaussian-Splatting>
)

target_link_directories(3D-Gaussian-Splatting PRIVATE ${PROJECT_SOURCE_DIR}/external/slang/lib)
target_link_libraries(3D-Gaussian-Splatting PRIVATE zlib gfx.lib slang.lib slang-rt.lib)

target_compile_definitions(3D-Gaussian-Splatting PRIVATE _USE_MATH_DEFINES)

if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif (UNIX)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_XLIB_KHR)
endif()

add_subdirectory(external/volk)
target_link_libraries(3D-Gaussian-Splatting PRIVATE volk)

if (MSVC)
    target_compile_options(3D-Gaussian-Splatting PRIVATE /W4 /WX)
else()
    target_compile_options(3D-Gaussian-Splatting PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
