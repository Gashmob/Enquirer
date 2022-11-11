cmake_minimum_required(VERSION 3.10)
project(Enquirer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

add_library(Enquirer_static STATIC enquirer/enquirer.hpp main.cpp)
set_target_properties(Enquirer_static PROPERTIES OUTPUT_NAME "Enquirer")
install(TARGETS Enquirer_static DESTINATION lib)

add_library(Enquirer_shared SHARED enquirer/enquirer.hpp main.cpp)
set_target_properties(Enquirer_shared PROPERTIES OUTPUT_NAME "Enquirer")
install(TARGETS Enquirer_shared DESTINATION lib)