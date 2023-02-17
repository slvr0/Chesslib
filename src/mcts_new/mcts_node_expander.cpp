#include "mcts_node_expander.h"

MCTSNodeExpansion::MCTSNodeExpansion(MCTSNodeInserter* node_inserter) :
    node_inserter_(node_inserter)
{

}

void MCTSNodeExpansion::Expand(MCTSNodeModel* from) {
    expansion_node_     = from;
    const Board board   = expansion_node_->GetBoard();
    
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1);
    else bmgen_.ParseLegalMoves(board, 1);  
}

void MCTSNodeExpansion::OnInsert(const Board& board, const int& depth) {  
    node_inserter_->CreateNodeModel(expansion_node_->GetInfo().GetDepth() + 1, board, expansion_node_);
}

MCTSVerboseNodeExpansion::MCTSVerboseNodeExpansion(MCTSNodeInserter* node_inserter) :
    node_inserter_(node_inserter)
{

}

void MCTSVerboseNodeExpansion::Expand(MCTSNodeModel* from) {
    expansion_node_     = from;
    const Board board   = expansion_node_->GetBoard();
    
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1, true);
    else bmgen_.ParseLegalMoves(board, 1, true);  
}

void MCTSVerboseNodeExpansion::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {
       
    node_inserter_->CreateNodeModel(expansion_node_->GetInfo().GetDepth() + 1, b2, info, expansion_node_);
}