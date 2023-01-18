#pragma once

#include <map>
#include <iostream>

#include "chessboard.h"
#include "move_generator.h"

struct DividerContextObject {

    DividerContextObject(const Board& board) :
        board_entry(board), entries(0) {
    }
    const Board           board_entry;
    unsigned long   entries;
};

class PerftDividerFactory : public MoveGeneratorHeader {
public :
    PerftDividerFactory() {
        
    }

    void            Enumerate(const Board & board, const int & maxdepth);
    unsigned long   Perft(const Board & board, const int & depth);
    void            OnInsert(const Board& board, const int& depth) override;
    void            OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) override;

private:
    std::map<std::string, DividerContextObject*> div_entries_;
    int max_depth_ = 0;

};