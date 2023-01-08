#pragma once

#include <iostream>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"

//creates monte carlo trees and sends search requests towards chesslib from incoming calls from comm lib

class ChesslibInterface {
public : 
    ChesslibInterface();
   

    void InitSearch(const Board &board, SearchType search_type, int maxdepth);

    void M_Callback(const Board& board, SearchType search_type, int depth);

    inline int GetEnumaratedMoves() { return n_moves_;}

private : 
    MoveGeneratorHeader mgen_;
    int n_moves_ = 0;
    int max_depth_ = 0;
};