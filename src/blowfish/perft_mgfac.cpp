#include "perft_mgfac.h"

int PerftMGFactory::Enumerate(const Board& board, const int& maxdepth) {
    found_moves_.clear();

    for(int i = 0 ; i < maxdepth + 1 ; ++i) found_moves_.push_back(0);

    maxdepth_       = maxdepth;
    n               = 0;
    const int depth = 0;    

    Perft(board, depth);

    print(Result());

    return n;
}

void PerftMGFactory::Perft(const Board& board, const int& depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
    else bmgen_.ParseLegalMoves(board, depth);
    
}

std::string PerftMGFactory::Result() const {
    std::string res = "";
    int depth = 0;
    for(const auto & entries : found_moves_) {   
        res += "Depth[" + std::to_string(depth++) + "] : " + std::to_string(entries) + "\n";
    }

    return res;
}

void PerftMGFactory::OnInsert(const Board& board, const int& depth) {
    ++n;

    found_moves_[depth] += 1;

    //std::cout << "depth : " << depth << " \t max : " << maxdepth_ << std::endl;
    if(depth < maxdepth_) {

        perft_calls++;
        Perft(board, depth);
    }
}