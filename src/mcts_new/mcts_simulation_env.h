#ifndef MCTS_SIMULATION_ENV
#define MCTS_SIMULATION_ENV

#include <future>
#include <memory>

#include "../blowfish/defs.h"

#include "mcts_defines.h"
#include "mcts_nodetree.h"


//connect some abort signal later
class MCTSSimulationEnvironment {
public:
    MCTSSimulationEnvironment();

    //gives back the ownership when finished
    std::unique_ptr<MCTSNodeTree> Search(std::unique_ptr<MCTSNodeTree>&    simulation_tree, const OptionsDict& opt);


private:
    std::unique_ptr<MCTSNodeTree>   simulation_tree_;
    OptionsDict                     params_;

    int undecisive  = 0;
    int whitewins   = 0;
    int blackwins   = 0;
    int draws       = 0;

    double delta_choose = 0;
    double delta_expand = 0;
    double delta_evaluate = 0;
};


#endif