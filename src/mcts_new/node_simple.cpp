#include "node_simple.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sstream>
/* 
NodeSimple::NodeSimple(const int &depth, const int &id, const Board& board, NodeSimple* parent) : 
    depth_(depth), id_(id), board_(board), is_white_(depth % 2 == 0), parent_(parent)
{

}

void NodeSimple::InsertNode(NodeSimple* node) {
    edges_.push_back(node);
}

void NodeSimple::InsertNode(const int &depth, const int &id, const Board& board, NodeSimple* parent) {
    this->InsertNode(new NodeSimple(depth, id, board, parent));
}

int NodeSimple::GetW() const {
    return Q_;
}

int NodeSimple::GetVisits() const {
    return N_;
}
int NodeSimple::GetId() const {
    return id_;
}

std::vector<NodeSimple*> NodeSimple::GetEdges() const {
    return edges_;
}

std::string NodeSimple::Debug() const {

}

bool NodeSimple::IsLeaf() const {
    return edges_.empty();
}
float NodeSimple::GetPolicyNonExpl() const {
    if(N_ == 0) return 100.f;
    return static_cast<float>(Q_) / N_;
}

float NodeSimple::GetPolicy() const {
    if(N_ == 0) return 100.f;

    const float ucbs = 0.41;
    float expl = parent_ ? ucbs * std::sqrt(std::log(parent_->GetVisits()) / (float) this->N_) : 0; // if called on null either root or fault

    return GetPolicyNonExpl() + expl;
}

std::string NodeSimple::DebugTree() const {
    std::string Nstr = "N:";
    return "(" + Nstr + std::to_string(this->N_) + "|Q:"+ std::to_string(this->Q_) + ")";
}

std::string NodeSimple::DebugTreeSimple() const {
    std::string color = is_white_ ? "W" : "B";
    std::stringstream sstream;
    sstream.precision(1);
    sstream << std::fixed;
    sstream << "(" + color + std::to_string(this->id_);
    sstream << "PE:";
    sstream << static_cast<float> (GetPolicy());
    sstream << "|P:";
    sstream << static_cast<float> (GetPolicyNonExpl());
    sstream << ")";
    return sstream.str();

}

void NodeSimple::UpdateResult(const SimResult& result) {
    ++N_;
    if(result == SimResult::WHITEWIN) {
        if(is_white_) ++Q_;
        else --Q_;
    }
    else if(result == SimResult::BLACKWIN) {
        if(is_white_) --Q_;
        else ++Q_;
    }

    if(parent_) parent_->UpdateResult(result);

}

void NodeSimple::Display(const int& indent, const bool& display_simple) const {
    for (int i = 0; i < indent; i++) {       
            std::cout << "\t";
    }

    if(display_simple) std::cout << DebugTreeSimple() << std::endl;
    else std::cout << DebugTree() << std::endl;
    

    if(!IsLeaf())
        for(int k = 0; k < edges_.size() ; ++k) edges_[k]->Display(indent + 1, display_simple);

}

NodeTree::NodeTree(const Board& board) {   
    root_ = std::unique_ptr<NodeSimple> (nexp_.CreateEdgeNode(nullptr, 0, board));
}

NodeSimple* NodeTree::CreateNode(NodeSimple* from, const int& depth, const Board& board) {
    return nexp_.CreateEdgeNode(from, depth, board);
}

void NodeExpander::PrintTreeStructure() const {
    int dlvl = 0;
    for(const auto & entries : levelentries_) {
        
        std::cout << "D:"<< dlvl++ << "|" << entries << std::endl;
    }
}

NodeSimple* NodeTree::Reset() const {
    return root_.get();
}

NodeSimple* NodeExpander::CreateEdgeNode(NodeSimple* from, const int &depth, const Board& board) {
    NodeSimple* node = new NodeSimple(depth, ids_++, board, from);
    if(from) from->InsertNode(node);        
    levelentries_[depth] += 1;
    return node;
}

void NodeExpander::IncrementRoot() {
    ++ids_;
}

NodeSimple* NodeSimple::GetBestPolicy() const {
    if(this->edges_.size() == 1) return this->edges_[0];

    return *std::max_element(this->edges_.begin(), this->edges_.end(), 
        [this](NodeSimple* n1, NodeSimple* n2) { return n1->GetPolicy() < n2->GetPolicy();});
}

NodeSimple* NodeSelect::GetId(NodeSimple* node, const int& sid) {
    if(node->GetId() == sid) return node;
    else {
        for(const auto & edge : node->GetEdges()) {
            NodeSimple* result = NodeSelect::GetId(edge, sid);
            if(result) return result;
        }
    }

    return nullptr;
}

void NodeRollout::Backpropagate(NodeSimple* node, const SimResult& outcome) {
    
}

SimResult NodeRollout::SimulateRandom(NodeSimple* node) {

}

void NodeTreeToolbox::CreateTreeFormationSimple(std::string position) {

    const int roolval   = 0;
    tree_               = std::make_unique<NodeTree> (0);
    
    NodeSelect ntree_search;
    int         entries = 0;
    const int   maxentries = 30;

    NodeSimple *nptr;

    while(entries < maxentries) {
        nptr = tree_->Reset();

        int d = 1;
        while(!nptr->IsLeaf()) {
            nptr = nptr->GetBestPolicy();
            ++d;
        }
       
        if(nptr->GetVisits() > 0) {
            tree_->CreateNode(nptr, d, 0); 
            ++entries;
            nptr = nptr->GetEdges()[0];
        }

        auto randres = rand() % 4; //replace this with a real evaluation


        nptr->UpdateResult(SimResult(randres));
    }
    
    //NodeSelect::GetId(tree_->Reset(), 3)->UpdateResult(SimResult::WHITEWIN);

    tree_->PrintTreeStructure();
    tree_->PrintTree(true);

}

 */

/* 
    NodeSimple* n1 = expander_.CreateEdgeNode(tree_->Reset(), 1, 0);
    NodeSimple* n2 = expander_.CreateEdgeNode(tree_->Reset(), 1, 0);
    NodeSimple* n3 = expander_.CreateEdgeNode(tree_->Reset(), 1, 0);

    auto n4 = expander_.CreateEdgeNode(n1, 2, 0);
    auto n5 = expander_.CreateEdgeNode(n4, 3, 0);

    auto n6 = expander_.CreateEdgeNode(n3, 2, 0);

    

    n5->UpdateResult(SimResult::WHITEWIN);
    n5->UpdateResult(SimResult::UNDECISIVE);
    n5->UpdateResult(SimResult::UNDECISIVE);
    n5->UpdateResult(SimResult::BLACKWIN);
    n5->UpdateResult(SimResult::BLACKWIN);

    n6->UpdateResult(SimResult::WHITEWIN);
    n6->UpdateResult(SimResult::WHITEWIN);
    n6->UpdateResult(SimResult::UNDECISIVE);
 */
