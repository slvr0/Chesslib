#pragma once

#include "move_generator.h"


class PerftMGFactory : public MoveGeneratorHeader {
public : 
    PerftMGFactory() {

    }

    int     Enumerate(const Board& board, const int& maxdepth);
    void    Perft(const Board& board, const int& depth);

    inline void OnInsert(const Board& board, const int& depth) {
        ++n;
        
        if(depth < maxdepth_) {
            Perft(board, depth);
        }
    }

private:
    int n = 0;
    int maxdepth_ = 0;
};