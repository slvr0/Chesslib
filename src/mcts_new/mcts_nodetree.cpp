#include "mcts_nodetree.h"

MCTSNodeTree::MCTSNodeTree(const Board& rootposition) : 
    tree_stats_(std::make_unique<MCTSNodeTreeStatistics> (20)) 
{
    node_inserter_ = std::make_unique<MCTSNodeInserter> (tree_stats_.get());
    root_ = std::unique_ptr<MCTSNodeModel> (node_inserter_->CreateNodeModel(0, rootposition, nullptr));
}

//dont need to pass depth just check parent depth + 1
MCTSNodeModel* MCTSNodeTree::InsertNode(const int& depth, const Board& board, MCTSNodeModel* parent) {
    return node_inserter_->CreateNodeModel(depth, board, parent);
}

std::string MCTSNodeTree::GetNodeTreeStatistics() const {
    return tree_stats_->CurrentTree();
}

MCTSNodeModel* MCTSNodeTree::Reset() const {
    return root_.get();
}

MCTSNodeInserter* MCTSNodeTree::GetNodeInserter() const {
    return node_inserter_.get();
}

int  MCTSNodeTree::GetTreeSize() const {
    return tree_stats_->GetEntries();
}

void MCTSNodeTree::DebugDisplayTree() const {
    //root_->DebugDisplay(0, true);
    root_->DebugDisplay(0, false);
}