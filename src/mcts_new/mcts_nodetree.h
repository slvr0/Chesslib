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
    ~MCTSNodeTree() = default;

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

    void ExpandVerbose(MCTSNodeModel* from);
    void ExpandNormal(MCTSNodeModel* from);

    std::string DebugMetrics() const;

    MCTSNodeModel* Slice(MCTSNodeModel* at);
    std::vector<MCTSNodeTree*> DisjointAllBranchesL1();
    void AttachSubTree(MCTSNodeTree* nodetree, const bool & atroot);
    void AttachSubTrees(std::vector<MCTSNodeTree*> nodetress, const bool& atroot=true);

private:
    std::unique_ptr<MCTSNodeModel>              root_           = nullptr;
    std::unique_ptr<MCTSNodeTreeStatistics>     tree_stats_     = nullptr;
    std::unique_ptr<MCTSNodeInserter>           node_inserter_  = nullptr;
    std::unique_ptr<MCTSNodeExpansion>          nexp_           = nullptr;
    std::unique_ptr<MCTSVerboseNodeExpansion>   verbose_nexp_   = nullptr;
};

#endif
