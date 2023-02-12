#include "mc_node_structure.h"

using namespace MCTS;

void NodeTreeStructure::Evaluate() {

    std::cout << "ID\tN\tV\tV/N\n";

    for(const auto & node : root_node_->edges_) {

        node->UpdateQFactor();
        std::cout << node->verbose_ << "\t" << node->N_ << "\t" << node->wl_  <<   std::endl;
    }
}