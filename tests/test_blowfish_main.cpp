#include "gtest/gtest.h"

//#include "test_perft_basic.cpp"   //instant
//#include "test_perft_medium.cpp"  //takes ~15-20 second (with debug on , 5-6 seconds otherwise)
//#include "test_perft_complex.cpp" //2 seconds

#include "test_perft_full.cpp" //minutes

#include "test_chessboard_fen.cpp" 

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}