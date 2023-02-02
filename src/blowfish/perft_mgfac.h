#pragma once

#include <vector>

#include "move_generator.h"

class PerftMGFactory : public MoveGeneratorHeader {
public : 
    PerftMGFactory() {
        
    }

    std::vector<int>    Enumerate(const Board& board,   const int& maxdepth);
    void                Perft(const Board& board,       const int& depth);
    std::string         Result() const;
    void                OnInsert(const Board& board,    const int& depth) override;

private:
    int n = 0;
    int maxdepth_ = 0;
    std::vector<int> found_moves_;
};