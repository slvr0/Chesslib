cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(blowfish_gtest
    test_blowfish_main.cpp

)

target_link_libraries(blowfish_gtest PRIVATE blowfish -lgtest)
