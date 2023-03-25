#ifndef NODEGRAPH_MANAGER_H
#define NODEGRAPH_MANAGER_H

#include <memory>

#include "../mcts_new/mcts_nodegraph.h"

#include "frozen_nodegraph.h"
#include "nodegraph_cache.h"

class NodeGraphManager {
public: 
    NodeGraphManager(NodeGraphCacheContainer* cache);
    FrozenNodeGraph* RetrieveCachedResult(NodeGraph* graph);

private:
    uint64_t GetNodeGraphKey(const Board& board);
    NodeGraphCacheContainer* cache_ = nullptr;
};


#endif