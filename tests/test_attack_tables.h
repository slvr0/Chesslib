#pragma once

#include "gtest/gtest.h"
#include "../src/core2/chess_board.h"
#include "../src/core2/attack_tables_handler.h"

#include "utils.h"



//move this one tomorrow 
TEST(AttackTableTest, CorrectAttackBishop_1) {
    
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/1P6/B3K3 w kq - 0 1";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  ASSERT_EQ(1,2);



  
}



/* GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
} */