#ifndef MCTS_TREESTRUCTURE_H
#define MCTS_TREESTRUCTURE_H

#include <vector>
#include <memory>

#include "mcts_nodegraph.h"
#include "mcts_nodeinserter.h"
#include "mcts_defines.h"
#include "mcts_node_expander.h"

//we should be able to trim head , which should automatically trigger a garbagecollection to reuse prev. tree head later.

class MCTSNodeTree {
public:
    MCTSNodeTree(const Board& rootposition);
    MCTSNodeTree(NodeGraph* node);
    ~MCTSNodeTree(); 

    NodeGraph* InsertNode(const int& depth, const Board& board, NodeGraph* parent = nullptr);
    std::string    GetNodeTreeStatistics() const;
    NodeGraph* Reset() const;

    MCTSNodeTree(const MCTSNodeTree&) = delete;
    MCTSNodeTree& operator=(const MCTSNodeTree&) = delete;

    MCTSNodeInserter* GetNodeInserter() const;
    void DebugDisplayTree() const;
    int GetTreeSize() const;

    NodeGraph* ReleaseHead();

    void MaybeExpandRoot();
    void ExpandNode(NodeGraph* from, bool verbose = false);


    void SetNodeInserter(MCTSNodeInserter* inserter);
    void DebugMetrics() const;

    NodeGraph* Slice(NodeGraph* at);
    std::vector<MCTSNodeTree*> DisjointAllBranchesL1();
    void AttachSubTree(MCTSNodeTree* nodetree, const bool & atroot);
    void AttachSubTrees(std::vector<MCTSNodeTree*> nodetress, const bool& atroot=true);

private:
    OptionsDict                                 params_;
    std::unique_ptr<NodeGraph>              root_           = nullptr;
    MCTSNodeTreeStatistics                      tree_stats_     { params_.kTreeStatisticPlyDepth };
    std::shared_ptr<MCTSNodeInserter>           node_inserter_  = nullptr;
    MCTSNodeExpansionHeader                     node_exp_header_ { nullptr };

    //deletes entire tree including head node
    void DeleteNodeModelChain(NodeGraph* node);
};

#endif
