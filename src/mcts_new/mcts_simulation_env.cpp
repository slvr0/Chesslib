#include "mcts_simulation_env.h"

#include "mcts_node_expander.h"
#include "mcts_evaluationtool.h"

MCTSSimulationEnvironment::MCTSSimulationEnvironment() {
    
}

void MCTSSimulationEnvironment::Search(MCTSNodeTree*   simulation_tree, const OptionsDict& opt) {        
    simulation_tree_ = simulation_tree;
    params_ = opt;

    NodeGraph *nodeptr;

    MCTS_SIM::MCTSModelEvaluation model_eval;

    Timer simul_timer;
    while(simulation_tree_->GetTreeSize() < params_.kSimulationEntries && 
          simul_timer.elapsed() < params_.kSimulationTimeout) {

        nodeptr = simulation_tree_->Reset();

        int d = 1;
        while(!nodeptr->IsLeaf()) { nodeptr = nodeptr->GetBestPolicy(); d++; }         

        if(nodeptr->GetVisits() > 0) {
            simulation_tree_->ExpandNode(nodeptr, d == 1 ? true : false);           

            if(nodeptr->IsLeaf()) { // still havent encountered yet for some reason
                print("found leaf, handle");
            }

            nodeptr = nodeptr->GetEdges()[0];
        }

        SimulationResult result = model_eval.SimulateGameplay(nodeptr->GetBoard(), params_);
        nodeptr->BackpropagateScoreUpdate(SimulationResult(result));  

        if(result       == SimulationResult::UNDECISIVE) undecisive++;
        else if(result  == SimulationResult::WHITEWIN) whitewins++;
        else if(result  == SimulationResult::BLACKWIN) blackwins++;
        else if(result  == SimulationResult::DRAW)     draws++;
    }
    
/*     std::cout << simulation_tree_->GetNodeTreeStatistics() << std::endl;

    std::cout << "W: " << std::to_string(whitewins) << " B: " << 
    std::to_string(blackwins) << " D: "<<std::to_string(draws) << " U: " << 
    std::to_string(undecisive) << std::endl;

    simulation_tree_->DebugMetrics();   */
}

