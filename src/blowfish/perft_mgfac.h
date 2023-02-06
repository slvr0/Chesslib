#pragma once

#include <vector>

#include "move_generator.h"

class PerftMGFactory : public MoveGeneratorHeader {
public : 
    PerftMGFactory() {
        
    }

    std::vector<unsigned long long>    Enumerate(const Board& board,   const int& maxdepth);
    
    inline void Perft(const Board& board,       const int& depth) {
        if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
        else bmgen_.ParseLegalMoves(board, depth);  
    }


    inline void OnInsert(const Board& board,    const int& depth) override {  
        found_moves_[depth] += 1;

        if(depth < maxdepth_) {
            Perft(board, depth);
        }
    }

    std::string Result() const;

private:
    int maxdepth_ = 0;
    std::vector<unsigned long long> found_moves_;
};