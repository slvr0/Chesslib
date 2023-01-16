#pragma once 

#include "gtest/gtest.h"

#include "../src/blowfish/defs.h"


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

TEST(BlackMoveGenerator, EnumarateBoard) {

    auto v1 = InsertBits(1,2,3);
    auto v2 = InsertBits(1,2,3);

    ASSERT_EQ(v1,v2);
}