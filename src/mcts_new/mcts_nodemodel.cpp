#include "mcts_nodemodel.h"

#include <cmath>
#include <sstream>

NodeInfo::NodeInfo(const int& depth, const int& id, std::string verbose) :
    depth_(depth), id_(id), verbose_(verbose)
{
    
}


int             NodeInfo::GetDepth() const { return depth_; }
int             NodeInfo::GetId() const { return id_; }
std::string     NodeInfo::GetMove() const { return verbose_; }

MCTSNodeModel::MCTSNodeModel(const Board& board, const NodeInfo& ninfo, MCTSNodeModel* parent) :
    board_(board), info_(ninfo), parent_(parent)
{

}

void MCTSNodeModel::InsertNode(MCTSNodeModel* node) {
    edges_.push_back(node);
}

void MCTSNodeModel::InsertNode(const Board& board, NodeInfo ninfo, MCTSNodeModel* parent) {
    this->InsertNode(new MCTSNodeModel(board, ninfo, parent));
}

void MCTSNodeModel::BackpropagateScoreUpdate(const SimulationResult& result) {
    ++N_;

    if(result == SimulationResult::WHITEWIN) {
        if(IsWhite()) ++WL_;
        else --WL_;
    }
    else if(result == SimulationResult::BLACKWIN) {
        if(IsWhite()) --WL_;
        else ++WL_;
    }
    else if(result == SimulationResult::DRAW) ++D_;

    if(parent_) parent_->BackpropagateScoreUpdate(result);
}

std::vector<MCTSNodeModel*> MCTSNodeModel::GetEdges() const {
    return edges_;
}

MCTSNodeModel* MCTSNodeModel::GetBestPolicy() const {
    if(this->edges_.size() == 1) return this->edges_[0];

    return *std::max_element(this->edges_.begin(), this->edges_.end(), 
        [this](MCTSNodeModel* n1, MCTSNodeModel* n2) { return n1->GetPolicy() < n2->GetPolicy();});
}

float   MCTSNodeModel::GetPolicyNonExpl() const {
    if(N_ == 0) return 100.f;
    return static_cast<float>(WL_) / N_ + static_cast<float>(D_*.05f) / N_;
}

float   MCTSNodeModel::GetPolicy() const {
    if(N_ == 0) return 100.f;

    const float ucbs = 0.41; // this needs to be evaluated
    float expl = parent_ ? ucbs * std::sqrt(std::log(parent_->GetVisits()) / (float) this->N_) : 0; // if called on null either root or fault

    return GetPolicyNonExpl() + expl;
}

NodeInfo MCTSNodeModel::GetInfo() const {
    return info_;
}

int     MCTSNodeModel::GetWinLooseRatio() const {
    return WL_;
}

int     MCTSNodeModel::GetDraws() const {
    return D_;
}

int     MCTSNodeModel::GetVisits() const {
    return N_;
}

bool    MCTSNodeModel::IsLeaf() const {
    return edges_.empty();
}

bool    MCTSNodeModel::IsWhite() const {
    return board_.white_acts_;
}

Board   MCTSNodeModel::GetBoard() const {
    return board_;
}

int     MCTSNodeModel::GetW()                          const {
    return WL_;
}

int     MCTSNodeModel::GetD()                          const {
    return D_;
}

MCTSNodeModel* MCTSNodeModel::GetParent()                  const {
    return parent_;
}

bool    MCTSNodeModel::RemoveParent() {
    bool detached = parent_->Detach(this);
    parent_ = nullptr;
    return detached;
}

bool    MCTSNodeModel::Detach(MCTSNodeModel* node) {
    auto it = std::find(edges_.begin(), edges_.end(), node);
    if ( it  != edges_.end()) {
        edges_.erase(it);
        return true;
    } return false;
}

//Debugging functions, visibility of node structure

void MCTSNodeModel::DebugDisplay(const int& indent, const bool& display_simple) const {
    for (int i = 0; i < indent; i++) {       
        std::cout << "\t";
    }

    if(display_simple) std::cout << NodeDisplaySimple() << std::endl;
    else std::cout << NodeDisplayFull() << std::endl;


    if(!IsLeaf())
        for(int k = 0; k < edges_.size() ; ++k) edges_[k]->DebugDisplay(indent + 1, display_simple);
}

std::string MCTSNodeModel::NodeDisplaySimple() const {
    std::string Nstr = "N:";
    return "(" + Nstr + std::to_string(this->N_) + "|W:"+ std::to_string(this->WL_) + "|D:" + std::to_string(this->D_) + ")";
}

std::string MCTSNodeModel::NodeDisplayFull()   const {
    std::string color = IsWhite() ? "W" : "B";
    std::stringstream sstream;
    sstream.precision(1);
    sstream << std::fixed;
    sstream << "(" + color + "-"+ this->GetInfo().GetMove() + "-";
    sstream << "PE:";
    sstream << static_cast<float> (GetPolicy());
    sstream << "|P:";
    sstream << static_cast<float> (GetPolicyNonExpl());
    sstream << ")";
    return sstream.str();
}
