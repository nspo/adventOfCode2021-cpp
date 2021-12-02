cmake_minimum_required(VERSION 3.10)
project(day02)

set(CMAKE_CXX_STANDARD 20) 

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(day02 main.cpp)
