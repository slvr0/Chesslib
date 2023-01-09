#pragma once

#include <iostream>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/perft_mgfac.h"

//creates monte carlo trees and sends search requests towards chesslib from incoming calls from comm lib

class ChesslibInterface {
public : 
    ChesslibInterface() = default;   

    void InitSearch(const Board &board, SearchType search_type, int maxdepth);

    void M_Callback(const Board& board, SearchType search_type, int depth);

    inline int GetEnumaratedMoves() { return n_moves_;}

private : 
    PerftMGFactory perft_factory_;
    int n_moves_ = 0;
    int max_depth_ = 0;
};