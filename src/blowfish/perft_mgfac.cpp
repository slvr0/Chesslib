#include "perft_mgfac.h"

std::vector<unsigned long long> PerftMGFactory::Enumerate(const Board& board, const int& maxdepth) {
    found_moves_.clear();

    for(int i = 0 ; i < maxdepth + 1 ; ++i) found_moves_.push_back(0);

    maxdepth_       = maxdepth;

    const int depth = 0; 
    found_moves_[0] = 1;

    Perft(board, depth);  

    return found_moves_;
}

std::string PerftMGFactory::Result() const {
    std::string res = "";
    int depth = 0;
    for(const auto & entries : found_moves_) {   
        res += "Depth[" + std::to_string(depth++) + "] : " + std::to_string(entries) + "\n";
    }

    return res;
}
