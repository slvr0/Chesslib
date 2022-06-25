#include "gtest/gtest.h"

//#include "test_board.h"
#include "test_attack_tables.h"

bool Chesslib::AttackTablesHandler::initialized = false;

GTEST_API_ int main(int argc, char **argv) {
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}