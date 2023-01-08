#include "movegen_interface.h"

ChesslibInterface::ChesslibInterface() {
    bool is_subscribed_to_mgen = mgen_.BindCallback([&](const Board& board, SearchType search_type, int depth) {
            return M_Callback(board, search_type, depth);
    } );   
}

void ChesslibInterface::InitSearch(const Board &board, SearchType search_type, int maxdepth){
    switch(search_type) {
        case SearchType::PERFT : {
            max_depth_ = maxdepth;
            n_moves_ = 0;

            int curr_depth = 0;
            mgen_.PruneSearch_PERFT(board, curr_depth);
        }
    }
}

void ChesslibInterface::M_Callback(const Board& board, SearchType search_type, int depth) {
    /* if(search_type == SearchType::PERFT && depth < max_depth_) {
        ++n_moves_;
    } */
}