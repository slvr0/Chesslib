cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_library(chesslib_test
    test_chesslib_main.cpp
    test_board.h
    test_board.cpp   
    test_attack_tables.h
    test_attack_tables.cpp
    test_move_generator.h
    test_move_generator.cpp
    utils.h
)

target_link_libraries(chesslib_test PRIVATE chesslib -lgtest)
