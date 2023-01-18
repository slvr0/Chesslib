cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(chess_interface
    movegen_interface.h
    movegen_interface.cpp

)

target_link_libraries(chess_interface PUBLIC blowfish_engine)

