#include "perft_mgfac.h"

int PerftMGFactory::Enumerate(const Board& board, const int& maxdepth) {
    maxdepth_       = maxdepth;
    n               = 0;
    const int depth = 0;    

    Perft(board, depth);



    return n;
}

void PerftMGFactory::Perft(const Board& board, const int& depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
    else bmgen_.ParseLegalMoves(board, depth);
    
}

void PerftMGFactory::OnInsert(const Board& board, const int& depth) {
    ++n;
    
    //std::cout << "depth : " << depth << " \t max : " << maxdepth_ << std::endl;
    if(depth < maxdepth_) {

        perft_calls++;
        Perft(board, depth);
    }
}