#ifndef MCTS_NODEINSERTER_H
#define MCTS_NODEINSERTER_H

#include "../blowfish/chessboard.h"

#include "mcts_nodegraph.h"

class MCTSNodeInserter {
public:
    MCTSNodeInserter(MCTSNodeTreeStatistics& tree_stats);   
    MCTSNodeInserter(); 

    NodeGraph* CreateNodeModel(const int& depth, const Board& board, NodeGraph* parent = nullptr);   
    NodeGraph* CreateNodeModel(const int& depth, const Board& board, std::string verbose, NodeGraph* parent = nullptr);  
    void InsertExistingNode(NodeGraph* node, NodeGraph* parent);  

private:
    MCTSNodeTreeStatistics* tree_stats_ = nullptr;
    int allocated_ids_ = 0;
};

#endif