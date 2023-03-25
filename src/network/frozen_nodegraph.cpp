#include "frozen_nodegraph.h"

FrozenNodeGraph::FrozenNodeGraph(uint64_t key, std::map<int, float>& probs, const float & value) : 
    key_(key), node_probs_(probs), node_value_(value)
{

}