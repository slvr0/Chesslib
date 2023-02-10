#pragma once 

#include <mutex>
#include <future>

#include "../blowfish/defs.h"

#include "mc_node_structure.h"
#include "mc_node.h"
#include "mc_simulator.h"
#include "mc_expansion.h"

namespace MCTS{
    
//might want a input signal to stop ponder
class MCExpandSimulateThread {
public : 

    MCExpandSimulateThread(NodeTreeStructure* nodetree) :
        nodetree_(nodetree) {
    }

    bool Ponder();

    std::future<bool> SpawnThread() {     
        return std::async(std::launch::async, [this] {return Ponder();});
    }

private:
    NodeTreeStructure*  nodetree_ = nullptr;
    MCSimulator         simulator_;
    MCMGenExpander      expander_;
    size_t              entries_ = 0;
    size_t              max_entries_ = 1e6;

    double delta_expansion = 0;
    double delta_nodecheckin = 0;
    double delta_simul = 0;
    double delta_checkout = 0;
};
}