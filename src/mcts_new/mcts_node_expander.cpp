#include "mcts_node_expander.h"

void MCTSNodeExpansion::Expand(NodeGraph* from) {
    m_assert(node_inserter_, "Node Expander have no Insertion tool attached");
    expansion_node_     = from;
    const Board board   = expansion_node_->GetBoard();
    
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1);
    else bmgen_.ParseLegalMoves(board, 1);  
}

void MCTSNodeExpansion::OnInsert(const Board& board, const int& depth) {  
    node_inserter_->CreateNodeModel(expansion_node_->GetInfo().GetDepth() + 1, board, expansion_node_);
}

void MCTSVerboseNodeExpansion::Expand(NodeGraph* from) {
    m_assert(node_inserter_, "Verbose Node Expander have no Insertion tool attached");
    expansion_node_     = from;
    const Board board   = expansion_node_->GetBoard();
    
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1, true);
    else bmgen_.ParseLegalMoves(board, 1, true);  
}

void MCTSVerboseNodeExpansion::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {
       
    node_inserter_->CreateNodeModel(expansion_node_->GetInfo().GetDepth() + 1, b2, info, expansion_node_);
}

void MCTSNodeExpansionHeader::ExpandNodeFull(NodeGraph* from, bool verbose) {
    if(verbose) verbose_expander_.Expand(from);
    else expander_.Expand(from);

    from->SetExpanded(true);
}