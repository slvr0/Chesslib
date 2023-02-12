#include "mc_node.h"

#include <sstream>

using namespace MCTS;

//recursively removes tree
void DeleteNode(MCTS::Node* node) {
    for(auto & branch : node->edges_) {
        DeleteNode(branch);
    }

    delete(node);
}

Node::Node(const Board & board, Node* parent) : 
        board_(board), parent_(parent) {
    UpdateNodePolicy();
}

Node::Node(const Board & board, std::string verbose, Node* parent) : 
        board_(board), verbose_(verbose), parent_(parent) {
    UpdateNodePolicy();
}

void Node::Insert(Node* node) {
    edges_.push_back(node);
}

void Node::Insert(const Board& board) {        
    edges_.push_back(new Node(board, this));
}

void Node::Insert(const Board& board, const std::string & verbose) {        
    edges_.push_back(new Node(board, verbose, this));
}

void Node::Backpropagate(GameResult result) {
    ++N_;
    if(result == GameResult::WhiteWin) {
        wl_ += this->board_.white_acts_ ? 1 : -1;
    } else if(result == GameResult::BlackWin) {
        wl_ += this->board_.white_acts_ ? -1 : 1;
    } else if(result  == GameResult::Draw) ++d_;
    
    UpdateNodePolicy();
    if(parent_) parent_->Backpropagate(result);
}

void Node::CheckIn() {    
    checkin_ = 0xFF; 
    UpdateNodePolicy();   
}

void Node::CheckOut() {
    checkin_ = 0x0;
    UpdateNodePolicy();
}

bool Node::IsLocked() const {  
    return static_cast<bool> (checkin_);
}

bool Node::IsLeaf() const {
    return edges_.empty();
}

std::string Node::Debug() const {
    std::ostringstream oss;
    oss << " Term:" << static_cast<int>(terminal_) 
        << " W:" << wl_ << " D:" << d_ << " N:" << N_ 
        << " Edges:" << static_cast<int>(edges_.size())
        << " Policy:"<< policy_
        << " Move:"  << verbose_;
    return oss.str();
}

//we can add boundary / terminal loss and other things here
void Node::UpdateQFactor() {
    if(N_ == 0) quality_ = 0; return;
    quality_ =  MCConfigurationParams::nwbias * ((float)wl_) + 
                MCConfigurationParams::ndbias * ((float)d_);
}

void Node::UpdateNodePolicy(const bool& add_dirichlet_noise) {
    if(this->IsLocked())    {policy_ = static_cast<float> (-1e5 ); return;}
    if(N_ == 0)             {policy_ = static_cast<float> ( 1e5 ); return;}

    policy_ = parent_ ? 
        MCConfigurationParams::nwbias * ((float)wl_ / N_) + 
        MCConfigurationParams::ndbias * ((float)d_  / N_) + 
        MCConfigurationParams::exploration_factor * std::sqrt((std::log(parent_->N_) / N_)) :

        MCConfigurationParams::nwbias * ((float)wl_ / N_) + 
        MCConfigurationParams::ndbias * ((float)d_  / N_);    
}

Node* Node::GetBestQNode(QualityFactorMethods qfmethod) const {
    std::function<bool(Node*, Node*)> qfuncptr;

    switch(qfmethod) {
        case QualityFactorMethods::NodeVisits : {
            qfuncptr = [this](Node*a, Node*b) {
                return (*a).N_ < (*b).N_;
            }; 
            break;
        }

        case QualityFactorMethods::NormalizedWinLooseRatio : {
            for(auto & edge : edges_) edge->UpdateQFactor();
            qfuncptr = [this](Node*a, Node*b) {
                return (*a).quality_ < (*b).quality_;
            };
            break;
        }
    }
    return *std::max_element(edges_.begin(), edges_.end(), qfuncptr);        
}

Node* Node::GetBestPNode() const {
    return *std::max_element(edges_.begin(), edges_.end(), 
        [](Node* a, Node*b) { return (*a).policy_ < (*b).policy_;});    
}

void Node::MakeTerminal(TerminalState terminal) {

}
