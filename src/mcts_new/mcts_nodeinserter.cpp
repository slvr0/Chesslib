#include "mcts_nodeinserter.h"

    MCTSNodeInserter::MCTSNodeInserter(MCTSNodeTreeStatistics& tree_stats) : 
        tree_stats_(&tree_stats)
    {

    }

    MCTSNodeInserter::MCTSNodeInserter() {

    }

    NodeGraph* MCTSNodeInserter::CreateNodeModel(const int& depth, const Board& board, NodeGraph* parent) {
        NodeInfo ninfo(depth, allocated_ids_++);
        NodeGraph* node = new NodeGraph(board, ninfo, parent);

        if(parent) parent->InsertNode(node);  //connect it
        if(tree_stats_) tree_stats_->UpdateWithInsertion(depth); //update tree statistics  
       
        return node;
    }

    NodeGraph* MCTSNodeInserter::CreateNodeModel(const int& depth, const Board& board, std::string verbose, NodeGraph* parent) {
        NodeInfo ninfo(depth, allocated_ids_++, verbose);
        NodeGraph* node = new NodeGraph(board, ninfo, parent);

        if(parent) parent->InsertNode(node);  //connect it
        if(tree_stats_) tree_stats_->UpdateWithInsertion(depth); //update tree statistics  
       
        return node;
    }

    void MCTSNodeInserter::InsertExistingNode(NodeGraph* node, NodeGraph* parent) {
        m_assert(parent, "Trying to insert a Node in Tree without parent");

        parent->InsertNode(node);
        if(tree_stats_) tree_stats_->UpdateWithInsertion(node->GetInfo().GetDepth()); //update tree statistics  
    } 
