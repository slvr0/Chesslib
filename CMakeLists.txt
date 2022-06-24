cmake_minimum_required(VERSION 3.10.2)

project(CMAKE_GENERIC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)

add_subdirectory(src)
add_subdirectory(tests)
add_executable(main main.cpp)

target_include_directories(main PUBLIC)

#after PRIVATE, add libraries you define in your src folder to link them to the main CMake.
target_link_libraries(main PUBLIC -lgtest chesslib chesslib_test -O3 -m64 -mfma -mavx2 -msse2 -msse3 -march=skylake -mpopcnt -funsafe-math-optimizations -fno-math-errno -std=c++2a -g -pthread)

include(CTest)

add_executable(
  chesslib_gtest
  tests/test_chesslib_main.cpp
  
)

target_link_libraries(chesslib_gtest PRIVATE -lgtest chesslib)

include(GoogleTest)
gtest_discover_tests(chesslib_gtest)