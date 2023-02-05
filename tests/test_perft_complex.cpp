#pragma once

#include <iostream>

#include "gtest/gtest.h"

#include "../src/blowfish/defs.h"
#include "../src/blowfish/perft_mgfac.h"
#include "../src/chess_interface/movegen_interface.h"
#include "../src/blowfish/perft_divider_autotraverse.h"


TEST(PerftComplexTests, ComplexPositions) {
    Board enp_pins1             = ChessboardGenerator::CreateFromFen("5q2/1NP1p3/8/2KP2rk/8/8/8/8 w - - 0 1");
    Board enp_discover_check    = ChessboardGenerator::CreateFromFen("4k3/3q4/8/8/8/1K6/2P5/8 w - - 0 1");
    Board push_block            = ChessboardGenerator::CreateFromFen("4k3/3q4/8/8/8/1K6/2P5/8 w - - 0 1");
    Board enp_hor_black         = ChessboardGenerator::CreateFromFen("8/8/8/8/Q4pk1/8/4P3/1K6 b - - 0 1");
    Board king_decimated_black  = ChessboardGenerator::CreateFromFen("3B4/1Q3pk1/6p1/8/6RR/4N3/8/1K6 w - - 0 1");
    Board king_decimated_white  = ChessboardGenerator::CreateFromFen("4r1k1/4r3/8/6n1/2q5/6p1/4PP1p/r2NK1N1 b - - 0 1");
    Board rook_replacement_cast = ChessboardGenerator::CreateFromFen("4k2r/5p2/6n1/7R/7r/8/5PPP/4K2R w Kk - 0 1");
    Board castle_into_check     = ChessboardGenerator::CreateFromFen("3r3b/3k4/6b1/8/8/4N3/4PPPP/R3K2R w KQ - 0 1");

    const int maxd = 4;

    PerftDividerAutotraverse perft_autotr;         
    bool res1 = perft_autotr.EnumerateDivide(enp_pins1, maxd);
    bool res2 = perft_autotr.EnumerateDivide(enp_discover_check, maxd);
    bool res3 = perft_autotr.EnumerateDivide(push_block, maxd);
    bool res4 = perft_autotr.EnumerateDivide(enp_hor_black, maxd);
    bool res5 = perft_autotr.EnumerateDivide(king_decimated_black, maxd);
    bool res6 = perft_autotr.EnumerateDivide(king_decimated_white, maxd);
    bool res7 = perft_autotr.EnumerateDivide(rook_replacement_cast, maxd);
    bool res8 = perft_autotr.EnumerateDivide(castle_into_check, maxd);

    ASSERT_EQ(res1, true);
    ASSERT_EQ(res2, true);
    ASSERT_EQ(res3, true);
    ASSERT_EQ(res4, true);
    ASSERT_EQ(res5, true);
    ASSERT_EQ(res6, true);
    ASSERT_EQ(res7, true);
    ASSERT_EQ(res8, true);    
}
