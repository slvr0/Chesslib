#pragma once 

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_node.h"



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

    inline void OnInsert(const Board& board, const int& depth) override 
    {
        m_assert(expand_node_, "Expansion node is not available");        
        
        expand_node_->Insert(board);
    }

private : 
    Node* expand_node_ = nullptr;
};