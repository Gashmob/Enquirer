cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(
        enquirer
        VERSION 1.0.2
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Don't use e.g. GNU extension (like -std=gnu++11) for portability
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_sources(${PROJECT_NAME}
        PRIVATE
        src/enquirer.cpp
)
target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
        include/enquirer.h
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

option(BUILD_TESTING "Build the testing tree." OFF)
if (BUILD_TESTING AND (PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
    target_compile_options(${PROJECT_NAME} PUBLIC
            -Wall
            -O0
            -g
    )
    enable_testing()
    add_subdirectory(tests)
else()
    target_compile_options(${PROJECT_NAME} PUBLIC
            -O3
    )
endif ()
