#pragma once

#include <iostream>

#include "../src/blowfish/chessboard.h"
#include "../src/blowfish/chessboard_extractor.h"
#include "../src/blowfish/defs.h"

#include "gtest/gtest.h"

TEST(ChessboardFenExtraction, GenerateFen) {

    std::string fen1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    std::string fen2 = "rnbqkbnr/pppppppp/8/8/6P1/8/PPPPPP1P/RNBQKBNR b KQkq - 0 1";
    std::string fen3 = "rnbqkbnr/1ppppppp/p7/8/6P1/8/PPPPPP1P/RNBQKBNR w KQkq - 0 2";
    std::string fen4 = "rnbqkbnr/1ppppppp/p7/6P1/8/8/PPPPPP1P/RNBQKBNR b kq h5 0 2";

    Board b1 = ChessboardGenerator::CreateFromFen(fen1);  
    Board b2 = ChessboardGenerator::CreateFromFen(fen2);
    Board b3 = ChessboardGenerator::CreateFromFen(fen3);
    Board b4 = ChessboardGenerator::CreateFromFen(fen4);

    ASSERT_EQ(BoardAsFen(b1) , fen1);
    ASSERT_EQ(BoardAsFen(b2) , fen2);
    ASSERT_EQ(BoardAsFen(b3) , fen3);
    ASSERT_EQ(BoardAsFen(b4) , fen4);

    std::string fen_nocast1 = "n1n5/PPP5/8/1k6/8/3K4/5ppp/5N1N w - - 3 3";
    Board b5 = ChessboardGenerator::CreateFromFen(fen_nocast1);

    ASSERT_EQ(BoardAsFen(b5) , fen_nocast1);

}