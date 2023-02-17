#include "mcts_nodeinserter.h"

    MCTSNodeInserter::MCTSNodeInserter(MCTSNodeTreeStatistics* tree_stats) : 
        tree_stats_(tree_stats)
    {

    }

    MCTSNodeModel* MCTSNodeInserter::CreateNodeModel(const int& depth, const Board& board, MCTSNodeModel* parent) {
        NodeInfo ninfo(depth, allocated_ids_++);
        MCTSNodeModel* node = new MCTSNodeModel(board, ninfo, parent);

        if(parent) parent->InsertNode(node);  //connect it
        if(tree_stats_) tree_stats_->UpdateWithInsertion(depth); //update tree statistics  
       
        return node;
    }

    MCTSNodeModel* MCTSNodeInserter::CreateNodeModel(const int& depth, const Board& board, std::string verbose, MCTSNodeModel* parent) {
        NodeInfo ninfo(depth, allocated_ids_++, verbose);
        MCTSNodeModel* node = new MCTSNodeModel(board, ninfo, parent);

        if(parent) parent->InsertNode(node);  //connect it
        if(tree_stats_) tree_stats_->UpdateWithInsertion(depth); //update tree statistics  
       
        return node;
    }
