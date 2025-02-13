cmake_minimum_required(VERSION 3.26)
project(CSE_340_Project_1)

set(CMAKE_CXX_STANDARD 11)

add_executable(CSE_340_Project_1 main.cpp
        DFA.cpp
        simple_parser.cpp
        shunting.cpp
        NFA.h)
