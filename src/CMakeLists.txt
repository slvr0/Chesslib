cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(blowfish_engine
    blowfish/chessboard.h
    blowfish/chessboard.cpp
    blowfish/chessboard_extractor.h
    blowfish/chessboard_extractor.cpp
    blowfish/static_move_tables.h
    blowfish/static_move_tables.cpp
    blowfish/position_meta_data.h
    blowfish/position_meta_data.cpp
    blowfish/move_generator.h
    blowfish/move_generator.cpp
    blowfish/chessboard_generator.h
    blowfish/chessboard_generator.cpp
    blowfish/defs.h
)
