#ifndef MCTS_TREESTRUCTURE_H
#define MCTS_TREESTRUCTURE_H

#include <vector>
#include <memory>

#include "mcts_nodemodel.h"
#include "mcts_nodeinserter.h"
#include "mcts_defines.h"

class MCTSNodeTree {
public:
    MCTSNodeTree(const Board& rootposition);
    ~MCTSNodeTree() = default;

    MCTSNodeModel* InsertNode(const int& depth, const Board& board, MCTSNodeModel* parent = nullptr);
    std::string    GetNodeTreeStatistics() const;
    MCTSNodeModel* Reset() const;

    MCTSNodeTree(const MCTSNodeTree&) = delete;
    MCTSNodeTree& operator=(const MCTSNodeTree&) = delete;

    MCTSNodeInserter* GetNodeInserter() const;
    void DebugDisplayTree() const;
    int GetTreeSize() const;

private:
    std::unique_ptr<MCTSNodeModel>          root_ = nullptr;
    std::unique_ptr<MCTSNodeTreeStatistics> tree_stats_ = nullptr;
    std::unique_ptr<MCTSNodeInserter>       node_inserter_ = nullptr;
};

#endif
