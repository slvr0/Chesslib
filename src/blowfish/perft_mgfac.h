#pragma once

#include "move_generator.h"


class PerftMGFactory : public MoveGeneratorHeader {
public : 
    PerftMGFactory() {

    }

    int     Enumerate(const Board& board, const int& maxdepth);
    void    Perft(const Board& board, const int& depth);

    void    OnInsert(const Board& board, const int& depth) override;

private:
    int n = 0;
    int maxdepth_ = 0;
    int perft_calls = 0;
};