#ifndef MCTS_NODE_EXPANDER_H
#define MCTS_NODE_EXPANDER_H

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mcts_nodemodel.h"
#include "mcts_nodeinserter.h"


class MCTSNodeExpansion : public MoveGeneratorHeader {
public: 
    MCTSNodeExpansion(MCTSNodeInserter* node_inserter);

    void Expand(MCTSNodeModel* from);
    void OnInsert(const Board& board, const int& depth);

private : 
    MCTSNodeInserter*   node_inserter_  = nullptr;
    MCTSNodeModel*      expansion_node_ = nullptr;
};

class MCTSVerboseNodeExpansion : public MoveGeneratorHeader {
public: 
    MCTSVerboseNodeExpansion(MCTSNodeInserter* node_inserter);

    void Expand(MCTSNodeModel* from);
    void OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) override;

private : 
    MCTSNodeInserter*   node_inserter_  = nullptr;
    MCTSNodeModel*      expansion_node_ = nullptr;
};

#endif