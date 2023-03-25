#ifndef FROZEN_NODEGRAPH_H
#define FROZEN_NODEGRAPH_H

#include <iostream>
#include <map>

class FrozenNodeGraph {
public : 
    FrozenNodeGraph(uint64_t key, std::map<int, float>& probs, const float & value);

    inline uint64_t                key() const { return key_;}
    inline std::map<int, float>    probabilites() const {return node_probs_;}
    inline float                   value() const {return node_value_;}

private:
    const uint64_t                key_;
    const std::map<int, float>    node_probs_;
    const float                   node_value_;    
};

#endif