#include "mcts_nodetree.h"

#include "../blowfish/defs.h"

MCTSNodeTree::MCTSNodeTree(const Board &rootposition) : tree_stats_(std::make_unique<MCTSNodeTreeStatistics>(20))
{
    node_inserter_ = std::make_unique<MCTSNodeInserter>(tree_stats_.get());
    root_ = std::unique_ptr<MCTSNodeModel>(node_inserter_->CreateNodeModel(0, rootposition, nullptr));

    nexp_ = std::make_unique<MCTSNodeExpansion>(node_inserter_.get());
    verbose_nexp_ = std::make_unique<MCTSVerboseNodeExpansion>(node_inserter_.get());
}

MCTSNodeTree::MCTSNodeTree(MCTSNodeModel *node) : tree_stats_(std::make_unique<MCTSNodeTreeStatistics>(20))
{
    node_inserter_ = std::make_unique<MCTSNodeInserter>(tree_stats_.get());
    root_ = std::unique_ptr<MCTSNodeModel>(node);

    nexp_ = std::make_unique<MCTSNodeExpansion>(node_inserter_.get());
    verbose_nexp_ = std::make_unique<MCTSVerboseNodeExpansion>(node_inserter_.get());
}

// dont need to pass depth just check parent depth + 1
MCTSNodeModel *MCTSNodeTree::InsertNode(const int &depth, const Board &board, MCTSNodeModel *parent)
{
    return node_inserter_->CreateNodeModel(depth, board, parent);
}

std::string MCTSNodeTree::GetNodeTreeStatistics() const
{
    return tree_stats_->CurrentTree();
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
    return tree_stats_->GetEntries();
}

void MCTSNodeTree::ExpandVerbose(MCTSNodeModel *from )
{
    verbose_nexp_->Expand(from);
}

void MCTSNodeTree::ExpandNormal(MCTSNodeModel *from)
{
    nexp_->Expand(from);
}

void MCTSNodeTree::MaybeExpandRoot()
{
    if (!root_->IsLeaf())
        return;

    ExpandVerbose(root_.get());
}

void MCTSNodeTree::DebugDisplayTree() const
{
    root_->DebugDisplay(0, false);
}

std::string MCTSNodeTree::DebugMetrics() const
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
        delete(nodetree);  
}

void MCTSNodeTree::AttachSubTrees(std::vector<MCTSNodeTree*> nodetrees, const bool & atroot) {
    for(auto & branch : nodetrees) {
        AttachSubTree(branch, atroot);
    }
}