#include "mcts_nodetree.h"

#include "../blowfish/defs.h"

MCTSNodeTree::MCTSNodeTree(const Board &rootposition) : 
    node_inserter_(std::make_shared<MCTSNodeInserter> (tree_stats_))    
{
    
    root_ = std::unique_ptr<MCTSNodeModel>(node_inserter_->CreateNodeModel(0, rootposition, nullptr));
    SetNodeInserter(node_inserter_.get());

}

MCTSNodeTree::MCTSNodeTree(MCTSNodeModel *node) :  
    node_inserter_(std::make_shared<MCTSNodeInserter> (tree_stats_))  
{   
    root_ = std::unique_ptr<MCTSNodeModel>(node);
    SetNodeInserter(node_inserter_.get());
}

MCTSNodeTree::~MCTSNodeTree() {
    //release inserter ( or not needed with shared ptr ? )
    
    //delete root tree
    auto rmn_ptr = root_.release();
    this->DeleteNodeModelChain(rmn_ptr);   
}

void MCTSNodeTree::SetNodeInserter(MCTSNodeInserter* inserter) {
    node_exp_header_.SetInserter(inserter);
}

// dont need to pass depth just check parent depth + 1
MCTSNodeModel *MCTSNodeTree::InsertNode(const int &depth, const Board &board, MCTSNodeModel *parent)
{
    return node_inserter_->CreateNodeModel(depth, board, parent);
}

std::string MCTSNodeTree::GetNodeTreeStatistics() const
{
    return tree_stats_.CurrentTree();
}

MCTSNodeModel *MCTSNodeTree::Reset() const
{
    return root_.get();
}

MCTSNodeInserter *MCTSNodeTree::GetNodeInserter() const
{
    return node_inserter_.get();
}

int MCTSNodeTree::GetTreeSize() const
{
    return tree_stats_.GetEntries();
}

void MCTSNodeTree::ExpandNode(MCTSNodeModel* from, bool verbose) {
    node_exp_header_.ExpandNodeFull(from, verbose);
}

void MCTSNodeTree::MaybeExpandRoot()
{
    if (!root_->IsLeaf())
        return;

    ExpandNode(root_.get(), true);
}

void MCTSNodeTree::DebugDisplayTree() const
{
    root_->DebugDisplay(0, false);
}

void MCTSNodeTree::DebugMetrics() const
{
    std::cout << "M\tW/L/R\tD\tN \tWLRD/N" << std::endl;
    for (auto &edge : root_->GetEdges())
    {
        std::cout << edge->GetInfo().GetMove() << "\t" << edge->GetW() << "\t" << edge->GetD() << "\t" << edge->GetVisits() << "\t" << edge->GetPolicyNonExpl() << std::endl;
    }
}

// we're not sure this node is in current tree? just assuming now, should have a compare function zobrist hash ?
MCTSNodeModel* MCTSNodeTree::Slice(MCTSNodeModel *at)
{
    m_assert(at->GetParent(), "trying to slice a node that is not connected");

    bool is_detached = at->RemoveParent();

    return at;
}

MCTSNodeModel* MCTSNodeTree::ReleaseHead() {
    return root_.release();
}
std::vector<MCTSNodeTree*> MCTSNodeTree::DisjointAllBranchesL1() {
    std::vector<MCTSNodeTree*> l1_br;
    for(auto & branch : this->root_->GetEdges()) {
        l1_br.push_back(new MCTSNodeTree(Slice(branch)));
    }
    return l1_br;
}

void MCTSNodeTree::AttachSubTree(MCTSNodeTree* nodetree, const bool & atroot) {
        if(atroot) node_inserter_->InsertExistingNode(nodetree->ReleaseHead(), this->root_.get());  

        //if we release head then this will trigger a delete on nullptr
        delete(nodetree);  
}

void MCTSNodeTree::AttachSubTrees(std::vector<MCTSNodeTree*> nodetrees, const bool & atroot) {
    for(auto & branch : nodetrees) {
        AttachSubTree(branch, atroot);
    }
}

void MCTSNodeTree::DeleteNodeModelChain(MCTSNodeModel* node) {
    if(!node) return;
    
    for(auto & branch : node->GetEdges()) {
        DeleteNodeModelChain(branch);
    }

    delete(node);
}