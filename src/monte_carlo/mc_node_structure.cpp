#include "mc_node_structure.h"

using namespace MCTS;

void NodeTreeStructure::Evaluate() {

    std::cout << "ID\tN\tV\tV/N\n";

    double norm = 0 ;
    for(auto & node : root_node_->edges_) {
        node->UpdateNodePolicy();
        norm += node->policy_;
    }
    for(auto & node : root_node_->edges_) {

        node->UpdateQFactor();
        
        std::cout << node->verbose_ << "\t" << node->N_ << "\t" << node->wl_ << "\t" << 
        (float) node->wl_ / node->N_ <<"\t"<< node->d_<< "\t"<< node->policy_ << std::endl;
    }
}