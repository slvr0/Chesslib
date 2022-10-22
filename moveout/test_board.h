#pragma once

#include "gtest/gtest.h"
#include "../src/core2/chess_board.h"
#include "../src/core2/attack_tables.h"

#include "utils.h"

//assures pieces are on the right squares
TEST(BoardSetFromFen, PiecesCorrectlyPositioned) {
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Chesslib::Board b1{pos1};

  uint64_t ourpawns = uint64t_frombits(std::initializer_list<uint8_t> {8,9,10,11,12,13,14,15});
  uint64_t enemypawns = uint64t_frombits(std::initializer_list<uint8_t> {48,49,50,51,52,53,54,55});

  auto our_pawns = b1.filter(b1.get(Chesslib::PAWN) , true);
  auto enemy_pawns = b1.filter(b1.get(Chesslib::PAWN) , false);

  ASSERT_EQ(ourpawns, our_pawns);
  ASSERT_EQ(enemypawns, enemy_pawns);
}

//assures filter appropriately shows each sides pieces
TEST(BoardFilter, FilterShowCorrectNrPieces_1) {
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  Chesslib::Board b1{pos1};
  auto our_pawns = b1.filter(b1.get(Chesslib::PAWN) , true);
  ASSERT_EQ(popcount_64(our_pawns), 8);
}

//assures filter appropriately shows each sides pieces
TEST(BoardFilter, FilterShowCorrectNrPieces_2) {
  std::string pos1 = "rnbqk1nr/pp1pb1pp/4p3/2P2P2/8/8/PPP2PPP/RNBQKBNR w KQkq - 1 5";

  Chesslib::Board b1{pos1};
  auto our_pawns = b1.filter(b1.get(Chesslib::PAWN) , true);
  auto enemy_pawns = b1.filter(b1.get(Chesslib::PAWN) , false);
  ASSERT_EQ(popcount_64(our_pawns), 8);
  ASSERT_EQ(popcount_64(enemy_pawns), 6);
}

//assures castling rights are correctly assessed when setting position from fen
TEST(SetFromFenCastlingState, CorrectCastlingState) {
  std::string pos1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  std::string pos2 = "rnbqk2r/pp3ppp/2pbpn2/3p4/2PP4/3BPN2/PP3PPP/RNBQ1RK1 b kq - 4 6";
  std::string pos3 = "2kr2nr/pbqn1ppp/1ppbp3/3p4/2PP4/1PNBPN2/PB3PPP/2RQK2R w K - 6 10";
  std::string pos4 = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";

  Chesslib::Board b1{pos1};
  Chesslib::Board b2{pos2};
  Chesslib::Board b3{pos3};
  Chesslib::Board b4{pos4};

  ASSERT_EQ(b1.getState().white_oo , true);
  ASSERT_EQ(b2.getState().black_ooo , true);
  ASSERT_EQ(b3.getState().white_ooo , false);
  ASSERT_EQ(b4.getState().black_ooo , false);
  
}


//assures the multiple switch paths are handled ok
/* TEST(PieceTypeTranslation, TranslateCorrectlyALL) {

  Chesslib::Board b1;
  ASSERT_EQ(b1.translatePieceType('p'), Chesslib::PieceType::PAWN);
  ASSERT_EQ(b1.translatePieceType('P'), Chesslib::PieceType::PAWN);
  ASSERT_EQ(b1.translatePieceType('b'), Chesslib::PieceType::BISHOP);
  ASSERT_EQ(b1.translatePieceType('B'), Chesslib::PieceType::BISHOP);
  ASSERT_EQ(b1.translatePieceType('n'), Chesslib::PieceType::KNIGHT);
  ASSERT_EQ(b1.translatePieceType('N'), Chesslib::PieceType::KNIGHT);
  ASSERT_EQ(b1.translatePieceType('r'), Chesslib::PieceType::ROOK);
  ASSERT_EQ(b1.translatePieceType('R'), Chesslib::PieceType::ROOK);
  ASSERT_EQ(b1.translatePieceType('q'), Chesslib::PieceType::QUEEN);
  ASSERT_EQ(b1.translatePieceType('Q'), Chesslib::PieceType::QUEEN);
  ASSERT_EQ(b1.translatePieceType('k'), Chesslib::PieceType::KING);
  ASSERT_EQ(b1.translatePieceType('K'), Chesslib::PieceType::KING);
  ASSERT_EQ(b1.translatePieceType('0'), Chesslib::PieceType::INVALID);
} */


//insert function and get() working
TEST(ChessBoardInsertPiece, InsertCorrectlyPawn) {  
  Chesslib::Board b1;

  b1.insertPiece(0, Chesslib::PieceType::BISHOP);
  b1.insertPiece(13, Chesslib::PieceType::PAWN);
  b1.insertPiece(16, Chesslib::PieceType::PAWN);
  b1.insertPiece(33, Chesslib::PieceType::PAWN);
  b1.insertPiece(45, Chesslib::PieceType::PAWN);
  b1.insertPiece(13, Chesslib::PieceType::PAWN);
  

  uint64_t p = b1.get(Chesslib::PieceType::PAWN);
  auto n_p = popcount_64(p);
  EXPECT_EQ(n_p , 5) << "total amount of pawns not matching \n";
}

TEST(ChessBoardInsertPiece, InsertCorrectlyRook) {  
  Chesslib::Board b1;

  b1.insertPiece(0, Chesslib::PieceType::ROOK);
  b1.insertPiece(13, Chesslib::PieceType::ROOK);
  b1.insertPiece(16, Chesslib::PieceType::ROOK);
  b1.insertPiece(63, Chesslib::PieceType::ROOK);
  b1.insertPiece(45, Chesslib::PieceType::ROOK);
  b1.insertPiece(13, Chesslib::PieceType::ROOK);
  

  uint64_t p = b1.get(Chesslib::PieceType::ROOK);
  auto n_p = popcount_64(p);
  EXPECT_EQ(n_p , 5) << "total amount of rooks not matching \n";
}

TEST(ChessBoardInsertPiece, InsertCorrectlyQueen) {  
  Chesslib::Board b1;

  b1.insertPiece(0, Chesslib::PieceType::QUEEN);
  b1.insertPiece(0, Chesslib::PieceType::QUEEN);
  b1.insertPiece(1, Chesslib::PieceType::QUEEN);
  b1.insertPiece(1, Chesslib::PieceType::QUEEN);
  b1.insertPiece(63, Chesslib::PieceType::QUEEN);
  b1.insertPiece(13, Chesslib::PieceType::QUEEN);
  

  uint64_t p = b1.get(Chesslib::PieceType::QUEEN);
  auto n_p = popcount_64(p);
  EXPECT_EQ(n_p , 4) << "total amount of queens not matching \n";
}

TEST(ChessBoardSetFromFen, SetFenCorrectPawns_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t p = b1.get(Chesslib::PieceType::PAWN);
  auto n_p = popcount_64(p);
  EXPECT_EQ(n_p , 16) << "total amount of pawns not matching \n";
}

TEST(ChessBoardSetFromFen, SetFenCorrectBishops_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t b = b1.get(Chesslib::PieceType::BISHOP);
  auto n_b = popcount_64(b);
  EXPECT_EQ(n_b , 4) << "total amount of bishops not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectKnights_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t kn = b1.get(Chesslib::PieceType::KNIGHT);
  auto n_kn = popcount_64(kn);
  EXPECT_EQ(n_kn , 4) << "total amount of knights not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectRooks_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t r = b1.get(Chesslib::PieceType::ROOK);
  auto n_r = popcount_64(r);
  EXPECT_EQ(n_r , 4) << "total amount of rooks not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectQueens_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t q = b1.get(Chesslib::PieceType::QUEEN);
  auto n_q = popcount_64(q);
  EXPECT_EQ(n_q , 2) << "total amount of queens not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectKings_1) { 
  std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t k = b1.get(Chesslib::PieceType::KING);
  auto n_k = popcount_64(k);   
  EXPECT_EQ(n_k , 2) << "total amount of kings not matching \n";

}


TEST(ChessBoardSetFromFen, SetFenCorrectPawns_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t p = b1.get(Chesslib::PieceType::PAWN);
  auto n_p = popcount_64(p);
  EXPECT_EQ(n_p , 11) << "total amount of pawns not matching \n";
}

TEST(ChessBoardSetFromFen, SetFenCorrectBishops_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t b = b1.get(Chesslib::PieceType::BISHOP);
  auto n_b = popcount_64(b);
  EXPECT_EQ(n_b , 2) << "total amount of bishops not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectKnights_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";     
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t kn = b1.get(Chesslib::PieceType::KNIGHT);
  auto n_kn = popcount_64(kn);
  EXPECT_EQ(n_kn , 4) << "total amount of knights not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectRooks_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";  ;    
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t r = b1.get(Chesslib::PieceType::ROOK);
  auto n_r = popcount_64(r);
  EXPECT_EQ(n_r , 4) << "total amount of rooks not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectQueens_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";     
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t q = b1.get(Chesslib::PieceType::QUEEN);
  auto n_q = popcount_64(q);
  EXPECT_EQ(n_q , 2) << "total amount of queens not matching \n";

}

TEST(ChessBoardSetFromFen, SetFenCorrectKings_2) { 
  std::string startpos = "r4rk1/pNp2pp1/7p/4n3/6q1/P2BPN1b/1PP1KPnP/R2Q2R1 w - - 6 17";     
  Chesslib::Board b1;
  b1.setFromFen(startpos);

  uint64_t k = b1.get(Chesslib::PieceType::KING);
  auto n_k = popcount_64(k);   
  EXPECT_EQ(n_k , 2) << "total amount of kings not matching \n";

}


/* 
//checks the setFromFen() setting approriate pieces from function
TEST(ChessBoardSetFromFen, SetFenPosition1_F) {
    std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Chesslib::Board b1;
    b1.setFromFen(startpos);
      
}

//checks the setFromFen() when setting from function after constructor already called the func
TEST(ChessBoardSetFromFen, SetFenPosition1_CF) {
    std::string startpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string startpos2 = "r1b1kb1r/p1p2ppp/1pnp4/3Pp1B1/8/3P1N2/PPP2PPP/R2QKB1R w KQkq - 0 8";
    Chesslib::Board b1{startpos};
    b1.setFromFen(startpos2);
     
} */





