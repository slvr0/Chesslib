#pragma once

#include <iostream>

#include "gtest/gtest.h"
#include "../src/core2/chess_board.h"
#include "../src/core2/attack_tables_handler.h"

#include "utils.h"

typedef std::initializer_list<uint8_t> Squares;

TEST(AttackTableTest, CorrectAttackBishop_1) {
    
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/1P6/B3K3 w kq - 0 1";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getBishopAttackPattern(b1.all(), 0);

  //Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares));

  Squares correct_squares {9};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackBishop_2) {
    
  std::string pos1 = "8/6k1/5n2/8/3B4/3NP3/3NK3/3N4 w - - 0 118";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getBishopAttackPattern(b1.all(), 27);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {0, 9, 18, 20, 36, 45, 34, 41, 48};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackBishop_3) {
    
  std::string pos1 = "1kb5/8/p7/8/8/2R4R/2K5/8 w - - 0 118";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getBishopAttackPattern(b1.all(), 58);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {49, 40, 51, 44, 37, 30, 23};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackBishop_4) {
    
  std::string pos1 = "5k2/6B1/2pppB2/6q1/1ppppp2/8/8/1K6 w - - 0 118";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getBishopAttackPattern(b1.all(), 45);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {52, 59, 54, 38, 36,27};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}


TEST(AttackTableTest, CorrectAttackBishop_5) {
    
  std::string pos1 = "6pB/3knpBp/3npBp1/3pBp2/2pBp3/2Bp4/8/3K4 w - - 0 118";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getBishopAttackPattern(b1.all(), 18);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {0, 9, 27 , 11 ,4, 25, 32};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}


TEST(AttackTableTest, CorrectAttackRooks_1) {
    
  std::string pos1 = "8/4k3/4q3/4b3/3P4/n2R4/3K4/8 w - - 0 0";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getRookAttackPattern(b1.all(), 19);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {16,17,18, 11,20,21,22,23,27};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackRooks_2) {
    
  std::string pos1 = "2kbn2r/3qpppp/8/8/8/8/4P3/4K3 w - - 0 1";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getRookAttackPattern(b1.all(), 63);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {55, 62,61,60};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackRooks_3) {
    
  std::string pos1 = "5bk1/8/8/8/6P1/B3RR2/K4p2/8 w - - 0 1";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getRookAttackPattern(b1.all(), 21);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {22,23, 13, 20, 29,37,45,53,61 };
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackRooks_4) {
    
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getRookAttackPattern(b1.all(), 0);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {1, 8 };
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}

TEST(AttackTableTest, CorrectAttackRooks_5) {
    
  std::string pos1 = "2k3r1/QR6/p1pq4/P2p4/5p1p/2P2P1P/1P4P1/7K b - - 0 37";
  Chesslib::Board b1{pos1};

  Chesslib::AttackTablesHandler attack_tables;

  Chesslib::U64 attack_squares = attack_tables.getRookAttackPattern(b1.all(), 49);

 /*  Chesslib::BoardConsoleGUI::printConsole(b1);
  Chesslib::BoardConsoleGUI::printConsole(Chesslib::Board(attack_squares)); */

  Squares correct_squares {48, 50,51,52,53,54,55,57, 41,33,25,17,9};
  auto correct_attacksquares = uint64t_frombits(correct_squares);

  ASSERT_EQ(attack_squares, correct_attacksquares);

}