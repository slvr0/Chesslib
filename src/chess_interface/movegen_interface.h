#pragma once

#include <iostream>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/perft_mgfac.h"
#include "../blowfish/perft_divider.h"

enum class SearchType {
    PERFT = 0,
    PERFT_DIVIDE = 1,


};

//Interface between user and move generator requests.

class ChesslibInterface {
public : 
    ChesslibInterface() = default;   

    int InitSearch(const Board &board, SearchType search_type, int maxdepth);

private :     

};