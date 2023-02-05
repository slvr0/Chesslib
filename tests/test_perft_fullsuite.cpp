#pragma once

#include <iostream>
#include <vector>
#include <fstream>

#include "gtest/gtest.h"

#include "../src/blowfish/defs.h"
#include "../src/blowfish/perft_mgfac.h"
#include "../src/chess_interface/movegen_interface.h"
#include "../src/blowfish/perft_divider_autotraverse.h"
//one of the most complete perft tests , this one takes a while. contains about 100 differnet positions with depth 6 analysis each with hundres of millions of nodes.

TEST(PerftFullTestSuite, FullSuite) {
    std::ifstream filereader;
    filereader.open("perftsuite.epd");

    std::string line;
    std::string delim = ";";

    ChesslibInterface chesslib_io;

    while(std::getline(filereader, line)) {
        std::vector<unsigned long long> expected_entries;
        std::string fenpos;
        
        auto segments = __SplitString(line, delim);

        fenpos = segments[0];

        for(int i = 1; i < segments.size(); ++i) {
            expected_entries.push_back(std::stoi(segments[i].substr(3, 12)));
        }

        Board position = ChessboardGenerator::CreateFromFen(fenpos);
        const int d = expected_entries.size();

        std::vector<unsigned long long> result = chesslib_io.InitSearch(position, SearchType::PERFT, d);
        for(int k = 0 ; k < d ; ++k) {
            print(fenpos);
            ASSERT_EQ(result[k + 1], expected_entries[k]);
        }
    }

}