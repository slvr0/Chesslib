#include "mcts_simulation_env.h"

#include "mcts_node_expander.h"

MCTSSimulationEnvironment::MCTSSimulationEnvironment() {
    
}

std::unique_ptr<MCTSNodeTree> MCTSSimulationEnvironment::Search(std::unique_ptr<MCTSNodeTree>&    simulation_tree, const OptionsDict& opt) {
    simulation_tree_ = std::move(simulation_tree);
    params_ = opt;

    int         entries = 0;
    const int   maxentries = 30;

    MCTSNodeModel *nodeptr;

    MCTSNodeExpansion           nexp(simulation_tree_->GetNodeInserter());
    MCTSVerboseNodeExpansion    verbose_nexp(simulation_tree_->GetNodeInserter());

    while(simulation_tree_->GetTreeSize() < maxentries) {
        nodeptr = simulation_tree_->Reset();

        int d = 1;
        while(!nodeptr->IsLeaf()) { nodeptr = nodeptr->GetBestPolicy(); d++; } 
       
        if(nodeptr->GetVisits() > 0) {
            if(d == 1) verbose_nexp.Expand(nodeptr); 
            else nexp.Expand(nodeptr);

            nodeptr = nodeptr->GetEdges()[0];
        }

        auto randres = rand() % 4; //replace this with a real evaluation
        nodeptr->BackpropagateScoreUpdate(SimulationResult(randres));
    }
    
    std::cout << simulation_tree_->GetNodeTreeStatistics() << std::endl;
    simulation_tree_->DebugDisplayTree();

    return std::move(simulation_tree_);    
}

