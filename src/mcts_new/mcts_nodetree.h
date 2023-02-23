#ifndef MCTS_TREESTRUCTURE_H
#define MCTS_TREESTRUCTURE_H

#include <vector>
#include <memory>

#include "mcts_nodemodel.h"
#include "mcts_nodeinserter.h"
#include "mcts_defines.h"
#include "mcts_node_expander.h"

//we should be able to trim head , which should automatically trigger a garbagecollection to reuse prev. tree head later.

class MCTSNodeTree {
public:
    MCTSNodeTree(const Board& rootposition);
    MCTSNodeTree(MCTSNodeModel* node);
    ~MCTSNodeTree(); 

    MCTSNodeModel* InsertNode(const int& depth, const Board& board, MCTSNodeModel* parent = nullptr);
    std::string    GetNodeTreeStatistics() const;
    MCTSNodeModel* Reset() const;

    MCTSNodeTree(const MCTSNodeTree&) = delete;
    MCTSNodeTree& operator=(const MCTSNodeTree&) = delete;

    MCTSNodeInserter* GetNodeInserter() const;
    void DebugDisplayTree() const;
    int GetTreeSize() const;

    MCTSNodeModel* ReleaseHead();

    void MaybeExpandRoot();
    void ExpandNode(MCTSNodeModel* from, bool verbose = false);


    void SetNodeInserter(MCTSNodeInserter* inserter);
    void DebugMetrics() const;

    MCTSNodeModel* Slice(MCTSNodeModel* at);
    std::vector<MCTSNodeTree*> DisjointAllBranchesL1();
    void AttachSubTree(MCTSNodeTree* nodetree, const bool & atroot);
    void AttachSubTrees(std::vector<MCTSNodeTree*> nodetress, const bool& atroot=true);

private:
    OptionsDict                                 params_;
    std::unique_ptr<MCTSNodeModel>              root_           = nullptr;
    MCTSNodeTreeStatistics                      tree_stats_     { params_.kTreeStatisticPlyDepth };
    std::shared_ptr<MCTSNodeInserter>           node_inserter_  = nullptr;
    MCTSNodeExpansionHeader                     node_exp_header_ { nullptr };

    //deletes entire tree including head node
    void DeleteNodeModelChain(MCTSNodeModel* node);
};

#endif
