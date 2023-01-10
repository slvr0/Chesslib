#include "movegen_interface.h"


int ChesslibInterface::InitSearch(const Board &board, SearchType search_type, int maxdepth){
    switch(search_type) {
        case SearchType::PERFT : {
            max_depth_ = maxdepth;
            n_moves_ = 0;

            int curr_depth = 0;
            int N = perft_factory_.Enumerate(board, maxdepth);   
            return N;     


        }
    }

    return 0;

}
