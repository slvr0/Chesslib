#include "movegen_interface.h"

void ChesslibInterface::InitSearch(const Board &board, SearchType search_type, int maxdepth){
    switch(search_type) {
        case SearchType::PERFT : {
            max_depth_ = maxdepth;
            n_moves_ = 0;

            int curr_depth = 0;
            int N = perft_factory_.Enumerate(board, maxdepth);   
            print(N);     
        }
    }
}

void ChesslibInterface::M_Callback(const Board& board, SearchType search_type, int depth) {
    /* if(search_type == SearchType::PERFT && depth < max_depth_) {
        ++n_moves_;
    } */
}