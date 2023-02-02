cmake_minimum_required(VERSION 3.10.2)

project(CHESSLIB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

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
#flags  -m64 -mfma -mavx2 -msse2 -msse3 -march=skylake  -funsafe-math-optimizations -fno-math-errno 
#-avx2 -mbmi2 
#-O3 -mpopcnt -mssse3 -march=native -mtune=generic -DPEXT -funsafe-math-optimizations -std=c++2a -g -pthread

#these run currently
#-O3 -mpopcnt -march=native -mtune=generic -DPEXT -funsafe-math-optimizations 

target_link_libraries(main PUBLIC -lgtest blowfish_engine chess_interface -O3 -mpopcnt -march=native -mtune=generic -DPEXT -funsafe-math-optimizations -std=c++2a  -g -pthread)

target_include_directories(blowfish_engine PUBLIC src/blowfish)
target_include_directories(chess_interface PUBLIC src/chess_interface)

include(CTest)

add_executable(
  blowfish_test
  tests/test_blowfish_main.cpp
  
)

target_link_libraries(blowfish_test PRIVATE -lgtest blowfish_engine chess_interface)

include(GoogleTest)
gtest_discover_tests(blowfish_test)