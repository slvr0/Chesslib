#pragma once

#include <iostream>

#include "gtest/gtest.h"

#include "../src/blowfish/defs.h"
#include "../src/blowfish/perft_mgfac.h"
#include "../src/chess_interface/movegen_interface.h"


TEST(PerftFullTest, OriginalChessPositionDepth6) { 
    Board startpos = ChessboardGenerator::CreateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    ChesslibInterface chesslib_io;

    const int maxd = 6;
    std::vector<int> result = chesslib_io.InitSearch(startpos, SearchType::PERFT, maxd);

    std::vector<int> correct {
        1,
        20,
        400,
        8902,
        197281,
        4865609,
        119060324
    };

    ASSERT_EQ(result[6] , correct[6]); 
} 



TEST(PerftFullTest, KiwiTestDepth5) { 
    Board startpos = ChessboardGenerator::CreateFromFen("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    ChesslibInterface chesslib_io;

    const int maxd = 5;
    std::vector<int> result = chesslib_io.InitSearch(startpos, SearchType::PERFT, maxd);

    std::vector<long long> correct {
        1,
        48,
        2039,
        97862,
        4085603,
        193690690,
        8031647685
    }; 

    ASSERT_EQ(result[5] , correct[5]);
}




TEST(PerftFullTest, PromotionBugDepth5) { 
    Board startpos = ChessboardGenerator::CreateFromFen("n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1");
    ChesslibInterface chesslib_io;

    const int maxd = 5;
    std::vector<int> result = chesslib_io.InitSearch(startpos, SearchType::PERFT, maxd);

    std::vector<int> correct {
        1,
        24,
        496,
        9483,
        182838,
        3605103      
    };

    ASSERT_EQ(result[5] , correct[5]); 
}

