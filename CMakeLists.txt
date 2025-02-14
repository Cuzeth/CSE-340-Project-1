cmake_minimum_required(VERSION 3.26)
project(CSE_340_Project_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(CSE_340_Project_1 main.cpp
        DFA.cpp
        shunting.cpp
        NFA.h
        NFA.cpp
        shunting.h)
