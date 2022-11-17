cmake_minimum_required(VERSION 3.10)
project(Enquirer)

set(CMAKE_CXX_STANDARD 14)

add_executable(demo
        tests/demo.cpp
        include/enquirer/enquirer.hpp)
target_include_directories(demo PUBLIC include/enquirer)

add_executable(test
        tests/test.cpp
        include/enquirer/enquirer.hpp tests/test.hpp)
target_include_directories(test PUBLIC include/enquirer)