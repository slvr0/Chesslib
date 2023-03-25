#include "mcts_nodegraph.h"

#include <cmath>
#include <sstream>

#include "../network/policy_index.h"

NodeInfo::NodeInfo(const int& depth, const int& id, std::string verbose) :
    depth_(depth), id_(id), verbose_(verbose)
{
       
}

int             NodeInfo::GetDepth() const { return depth_; }
int             NodeInfo::GetId() const { return id_; }
std::string     NodeInfo::GetMove() const { return verbose_; }

NodeGraph::NodeGraph(const Board& board, const NodeInfo& ninfo, NodeGraph* parent) :
    board_(board), info_(ninfo), parent_(parent)
{
    SetPolicyIndex(); 
    SetHistory();
}

void NodeGraph::InsertNode(NodeGraph* node) {
    edges_.push_back(node);
}

void NodeGraph::InsertNode(const Board& board, NodeInfo ninfo, NodeGraph* parent) {
    this->InsertNode(new NodeGraph(board, ninfo, parent));
}

void NodeGraph::BackpropagateScoreUpdate(const SimulationResult& result) {
    ++N_;

    if(result == SimulationResult::WHITEWIN) {
        if(IsWhite()) ++W_;
        else ++L_;
    }
    else if(result == SimulationResult::BLACKWIN) {
        if(IsWhite()) ++L_;
        else ++W_;
    }
    else if(result == SimulationResult::DRAW) ++D_;

    if(parent_) parent_->BackpropagateScoreUpdate(result);
}

std::vector<NodeGraph*> NodeGraph::GetEdges() const {
    return edges_;
}

NodeGraph* NodeGraph::GetBestPolicy() const {
    if(this->edges_.size() == 1) return this->edges_[0];

    return *std::max_element(this->edges_.begin(), this->edges_.end(), 
        [this](NodeGraph* n1, NodeGraph* n2) { return n1->GetPolicy() < n2->GetPolicy();});
}

float NodeGraph::GetPolicyNonExpl() const {
    if(N_ == 0) return 100.f;
    return P_;
}

float NodeGraph::GetPolicy() const {
    if(N_ == 0) return 100.f;

    const float ucbs = 0.41; // this needs to be evaluated
    float expl = parent_ ? ucbs * std::sqrt(std::log(parent_->GetVisits()) / (float) this->N_) : 0; // if called on null either root or fault

    return GetPolicyNonExpl() + expl;
}

NodeInfo NodeGraph::GetInfo() const {
    return info_;
}

bool NodeGraph::IsLeaf() const {
    return edges_.empty();
}

bool NodeGraph::IsWhite() const {
    return board_.white_acts_;
}

int NodeGraph::GetW() const {
    return W_;
}

int NodeGraph::GetD() const {
    return D_;
}

int NodeGraph::GetL() const {
    return L_;
}

int NodeGraph::GetVisits() const {
    return N_;
}

Board NodeGraph::GetBoard() const {
    return board_;
}

void NodeGraph::SetP(float value) {
    P_ = value;
}

float NodeGraph::GetP() const {
    return P_;
}

void NodeGraph::SetQ(float value) {
    Q_ = value;
}

float NodeGraph::GetQ() const {
    return Q_;
}

NodeGraph* NodeGraph::GetParent() const {
    return parent_;
}

bool NodeGraph::RemoveParent() {
    bool detached = parent_->Detach(this);
    parent_ = nullptr;
    return detached;
}

bool    NodeGraph::Detach(NodeGraph* node) {
    auto it = std::find(edges_.begin(), edges_.end(), node);
    if ( it  != edges_.end()) {
        edges_.erase(it);
        return true;
    } return false;
}

std::vector<std::string> NodeGraph::GetHistory() const {
    return history_;
}

//Debugging functions, visibility of node structure
void NodeGraph::SetExpanded(bool v) {
    expanded_ = v;
}

bool NodeGraph::GetExpanded() const {
    return expanded_;
}

void NodeGraph::SetFrozen() {
    frozen_ = true;
}

bool NodeGraph::GetFrozen() {
    return frozen_;
}

void NodeGraph::SetHistory() {
    if(parent_) {
        this->history_ = parent_->GetHistory();
        this->history_.insert(this->history_.begin(), this->info_.verbose_);
    }
}

void NodeGraph::SetHistory(std::vector<std::string> history) {
    this->history_ = history;
}

void NodeGraph::SetPolicyIndex() {
    std::string move = info_.verbose_;
    if(move == "") return;

    auto ret = std::find(std::begin(policy_index), std::end(policy_index), move);

    if(ret != std::end(policy_index)) policy_index_ = std::distance(policy_index, ret);
}

int NodeGraph::GetPolicyIndex() const {
    return policy_index_;
}

void NodeGraph::DebugDisplay(const int& indent, const bool& display_simple) const {
    for (int i = 0; i < indent; i++) {       
        std::cout << "\t";
    }

    if(display_simple) std::cout << NodeDisplaySimple() << std::endl;
    else std::cout << NodeDisplayFull() << std::endl;

    if(!IsLeaf())
        for(int k = 0; k < edges_.size() ; ++k) edges_[k]->DebugDisplay(indent + 1, display_simple);
}

std::string NodeGraph::NodeDisplaySimple() const {
    std::string Nstr = "N:";
    return "(" + Nstr + std::to_string(this->N_) + "|Q:" + std::to_string(this->Q_) +  "|P:" + std::to_string(this->P_) + "|W:"+ std::to_string(this->W_) + "|D:" + std::to_string(this->D_) + ")";
}

std::string NodeGraph::NodeDisplayFull() const {
    std::string color = IsWhite() ? "W" : "B";
    std::stringstream sstream;
    sstream.precision(1);
    sstream << std::fixed;
    sstream << "(" + color + "-"+ this->GetInfo().GetMove() + "-";
    sstream << "PE:";
    sstream << static_cast<float> (GetPolicy());
    sstream << "|P:";
    sstream << static_cast<float> (P_);
    sstream << "|Q:";
    sstream << static_cast<float> (GetQ());
    sstream << ")";
    return sstream.str();
}
