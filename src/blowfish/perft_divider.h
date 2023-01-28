#pragma once

#include <map>
#include <iostream>
#include <memory>

#include "chessboard.h"
#include "move_generator.h"



class BoardNode {
public : 
    BoardNode(const Board& board) : board_(board){

    }

    BoardNode(BoardNode* node) : board_(node->board_) {

    }

    inline void InsertNode(const Board& board) {
        branches_.push_back(new BoardNode(board));
    }

    inline void InsertNode(BoardNode* node) {
        branches_.push_back(node);
    }

    inline bool IsLeaf() {
        return branches_.empty();
    }

    inline size_t GetEntries() const { return branches_.size();}

    Board                   board_;
    std::string             tag_ = " ";
    std::vector<BoardNode*> branches_;

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
    int CountNodesAndLeafFrom(BoardNode* node);

private:
    int max_depth_ = 0; 
    int n = 0;
    std::vector<BoardNode*> next_nodelist_;
    BoardNode* brdptr_ = nullptr;


};