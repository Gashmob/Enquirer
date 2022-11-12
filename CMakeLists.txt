cmake_minimum_required(VERSION 3.10)
project(Enquirer)

set(CMAKE_CXX_STANDARD 14)

add_executable(Enquirer
        tests/main.cpp
        include/enquirer/enquirer.hpp)
target_include_directories(Enquirer PUBLIC include/enquirer)