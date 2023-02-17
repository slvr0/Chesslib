#include "mcts_simulation_env.h"

#include "mcts_node_expander.h"
#include "mcts_evaluationtool.h"

MCTSSimulationEnvironment::MCTSSimulationEnvironment() {
    
}

std::unique_ptr<MCTSNodeTree> MCTSSimulationEnvironment::Search(std::unique_ptr<MCTSNodeTree>&    simulation_tree, const OptionsDict& opt) {
    simulation_tree_ = std::move(simulation_tree);
    params_ = opt;

    int         entries = 0;
    const int   maxentries = 20000;

    MCTSNodeModel *nodeptr;

    MCTSNodeExpansion           nexp(simulation_tree_->GetNodeInserter());
    MCTSVerboseNodeExpansion    verbose_nexp(simulation_tree_->GetNodeInserter());
    MCTS_SIM::MCTSModelEvaluation model_eval;

    while(simulation_tree_->GetTreeSize() < maxentries) {
        nodeptr = simulation_tree_->Reset();

        int d = 1;
        while(!nodeptr->IsLeaf()) { nodeptr = nodeptr->GetBestPolicy(); d++; } 
       
        if(nodeptr->GetVisits() > 0) {
            if(d == 1) verbose_nexp.Expand(nodeptr); 
            else nexp.Expand(nodeptr);

            nodeptr = nodeptr->GetEdges()[0];
        }

        SimulationResult result = model_eval.SimulateGameplay(nodeptr->GetBoard(), params_);
        if(result == SimulationResult::UNDECISIVE) undecisive++;
        else if(result == SimulationResult::WHITEWIN) whitewins++;
        else if(result == SimulationResult::BLACKWIN) blackwins++;
        else if(result == SimulationResult::DRAW)     draws++;

        nodeptr->BackpropagateScoreUpdate(SimulationResult(result));

      

    }
    
    std::cout << simulation_tree_->GetNodeTreeStatistics() << std::endl;
    //simulation_tree_->DebugDisplayTree();


    std::cout << "W: " << std::to_string(whitewins) << " B: " << 
    std::to_string(blackwins) << " D: "<<std::to_string(draws) << " U: " << 
    std::to_string(undecisive) << std::endl;

    return std::move(simulation_tree_);    
}

