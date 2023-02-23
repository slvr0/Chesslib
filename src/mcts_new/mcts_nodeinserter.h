#ifndef MCTS_NODEINSERTER_H
#define MCTS_NODEINSERTER_H

#include "../blowfish/chessboard.h"

#include "mcts_nodemodel.h"

class MCTSNodeInserter {
public:
    MCTSNodeInserter(MCTSNodeTreeStatistics& tree_stats);   
    MCTSNodeInserter(); 

    MCTSNodeModel* CreateNodeModel(const int& depth, const Board& board, MCTSNodeModel* parent = nullptr);   
    MCTSNodeModel* CreateNodeModel(const int& depth, const Board& board, std::string verbose, MCTSNodeModel* parent = nullptr);  
    void InsertExistingNode(MCTSNodeModel* node, MCTSNodeModel* parent);  

private:
    MCTSNodeTreeStatistics* tree_stats_ = nullptr;
    int allocated_ids_ = 0;
};

#endif