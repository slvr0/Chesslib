#pragma once 

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_node.h"

namespace MCTS {
class MCMGenExpander : public MoveGeneratorHeader {
public: 
    MCMGenExpander() {

    }

    inline void Expand(Node* node) {
        expand_node_        = node;
        const Board board   = expand_node_->board_;
        
        if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1);
        else bmgen_.ParseLegalMoves(board, 1);  
    }

    inline void OnInsert(const Board& board, const int& depth) override {               
        expand_node_->Insert(board);
    }

private : 
    Node* expand_node_ = nullptr;
};

class MCMGenVerboseExpander : public MoveGeneratorHeader {
public: 
    MCMGenVerboseExpander() {

    }

    inline void Expand(Node* node) {
        expand_node_        = node;
        const Board board   = expand_node_->board_;
        
        if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1, true);
        else bmgen_.ParseLegalMoves(board, 1, true);  
    }

    inline virtual void OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) override {
        expand_node_->Insert(b2, info);
    }

private : 
    Node* expand_node_ = nullptr;
};
}