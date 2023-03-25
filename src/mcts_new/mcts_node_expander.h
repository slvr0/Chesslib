#ifndef MCTS_NODE_EXPANDER_H
#define MCTS_NODE_EXPANDER_H

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mcts_nodegraph.h"
#include "mcts_nodeinserter.h"


class MCTSNodeExpansion : public MoveGeneratorHeader {
    friend class MCTSNodeExpansionHeader;
public:

    MCTSNodeExpansion() = default;

    void Expand(NodeGraph* from);
    void OnInsert(const Board& board, const int& depth);

private:
    inline void SetInserterTool(MCTSNodeInserter* node_inserter) {
        node_inserter_ = node_inserter;
    }
private : 
    MCTSNodeInserter*   node_inserter_  = nullptr;
    NodeGraph*      expansion_node_ = nullptr;
};

class MCTSVerboseNodeExpansion : public MoveGeneratorHeader {
    friend class MCTSNodeExpansionHeader;
public: 
    MCTSVerboseNodeExpansion() = default;

    void Expand(NodeGraph* from);
    void OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) override;

private:
    inline void SetInserterTool(MCTSNodeInserter* node_inserter) {
        node_inserter_ = node_inserter;
    }

private : 
    MCTSNodeInserter*   node_inserter_  = nullptr;
    NodeGraph*      expansion_node_ = nullptr;
};

class MCTSNodeExpansionHeader {
public:
    MCTSNodeExpansionHeader(MCTSNodeInserter* node_inserter = nullptr) {
        SetInserter(node_inserter);
    }

    void ExpandNodeFull(NodeGraph* from, bool verbose = true);

    void SetInserter(MCTSNodeInserter* node_inserter) {
        if(node_inserter) {
            expander_.SetInserterTool(node_inserter);
            verbose_expander_.SetInserterTool(node_inserter);
        }
    }

private:
    MCTSNodeInserter*           node_inserter_;
    MCTSNodeExpansion           expander_;
    MCTSVerboseNodeExpansion    verbose_expander_;
};

#endif