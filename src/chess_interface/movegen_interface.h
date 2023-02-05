#pragma once

#include <iostream>

#include "../blowfish/chessboard.h"

enum class SearchType {
    PERFT = 0,
    PERFT_DIVIDE = 1,


};

//Interface between user and move generator requests.

class ChesslibInterface {
public : 
    ChesslibInterface() = default;   

    std::vector<unsigned long long> InitSearch(const Board &board, SearchType search_type, int maxdepth);

private :     

};