cmake_minimum_required(VERSION 3.24)
project(Chip8)

set(CMAKE_CXX_STANDARD 17)

include_directories(FailStates Chip8 Machine)
add_executable(
        Chip8
        main.cpp
        Chip8/Chip8.cpp
        Machine/Machine.cpp Machine/Machine.h)

TARGET_LINK_LIBRARIES(Chip8 sfml-graphics sfml-window sfml-system)