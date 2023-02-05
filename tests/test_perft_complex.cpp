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


//MB add later
//Black Positions

//BP1 = 7k/pp1b1r2/2p5/3p3p/5Ppq/P2BQ2N/1P4P1/5R1K b - - 0 1
//BP2 = 1q6/1P4R1/4Nn1k/8/2K5/5rP1/2B5/8 b - - 0 1
//BP3 = 4R3/p3k1p1/1P1N3p/P3b3/8/5P1P/6P1/5K2 b - - 2 55
//BP4 = 2k4r/1p3Bpp/p4n2/4p3/4P3/5PKP/PPn5/1NR5 b - - 1 22
//BP5 = r2q1rk1/4bpp1/p1b1pn1p/1pPpB3/1P1P4/P1NB3P/2Q2PP1/4RRK1 b - - 2 17
//BP6 = 8/8/2kpb3/8/3NB3/4K3/8/8 b - - 2 55
//BP7 = 8/8/2k5/8/8/K7/p1p3p1/2RN2N1 b - - 2 55
//BP8 = n6n/R1pk1P2/3pp2q/5Pb1/8/8/6K1/n6n b - - 2 55
//BP9 = 8/8/8/1k6/1pP5/8/1R6/4K3 b - c3 0 55
//BP10 = 8/8/8/8/kpP1RR2/8/8/4K3 b - - c3 55
//BP11 = 4Q3/3r4/2k3nR/3p4/8/5B2/2pNN3/4K3 b - - 2 55

//investigated during development
/* 
std::string opendiag = "rnbqkbnr/pp2pppp/2p5/3p4/3P4/6P1/PPP1PP1P/RNBQKBNR w KQkq - 0 3";
std::string open_diag_rooks = "rnbqkbnr/p4p1p/4p3/1Ppp2p1/3P3P/5NP1/1PP1PP2/RNBQKB1R w KQkq - 0 7";

std::string checkpin_pos_00 = "1k6/8/8/8/8/3K2r1/8/8 w - - 0 1";
std::string checkpin_pos_01 = "1k6/8/8/5b2/8/3KP1r1/8/8 w - - 0 1";
std::string pin_test_00 = "2k5/2p4b/8/8/4Q3/3K4/8/8 w - - 0 1";
std::string pin_test_01 = "2k5/2p4b/8/8/2b1Q3/3K4/8/8 w - - 0 1"; 
std::string pin_test_02 = "2k5/2p4b/8/1p6/2b1Q3/3K4/8/8 w - - 0 1";

std::string pawn_movement_test_01 = "1k6/ppp1P2p/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 1"; //assert correct!
std::string pawn_movement_test_02 = "1k6/ppp1P3/8/2p3Pp/3P1p2/1K6/1P3P2/8 w - h6 0 2"; //assert correct!
std::string pawn_movement_test_022 = "1k4nn/ppp1P1Pp/8/2p2pP1/3P1p2/1K6/1P3P2/8 w - f6 0 2"; //assert correct!
std::string pawn_movement_test_03 = "1k4bn/ppp1P1Pp/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 1"; //pawn asserts correct, king moves absence?

std::string pawn_movement_test_04 = "1k4nn/ppp1P1Pp/8/2p3P1/3P1p2/1K6/1P3P2/8 w - - 0 11"; //assert correct!    
std::string pawn_movement_test_05 = "1k2b1nn/ppp2PPp/8/2p3P1/K2P1p2/8/1P3P2/8 w - - 0 1"; //assert correct!

std::string pawn_difficult_enp_01 = "5k2/8/8/1K1Pp1q1/8/8/8/8 w - e6 0 2";

std::string pawn_difficult_enp_02 = "5k2/8/8/1K1pP1q1/8/8/8/8 w - d6 0 2";

std::string castle_01 = "2k4r/8/8/8/8/8/4PPP1/R3K2R w KQk - 0 1";
 */