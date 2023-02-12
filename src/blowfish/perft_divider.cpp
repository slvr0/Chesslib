#include "perft_divider.h"
#include "perft_mgfac.h"

void PerftDividerFactory::CountNodesAndLeafFrom(BoardNode* node, int & count) {     
    
    //trying to avoid terminal nodes
    
    for(auto & child : node->GetBranches()) {
/*         In perft, nodes are only counted at the end after the last makemove. 
        Thus "higher" terminal nodes (e.g. mate or stalemate) are not counted, instead the number of move paths of a certain depth.  */
        if(child->IsLeaf() && !child->IsTerminal()) {
            ++count;       
        }
        else {
            CountNodesAndLeafFrom(child, count);   
        }        
    }  
}

std::map<std::string, BoardNode*> PerftDividerFactory::Enumerate(const Board & board, const int & maxdepth) { 
    std::map<std::string, BoardNode*> map_divider_entries;  

    max_depth_ = maxdepth;

    root_ = std::make_unique<BoardNode>(new BoardNode(board));
    //auto root = new BoardNode(board);

    std::vector<BoardNode*> curr_nodelist {root_.get()};
    next_nodelist_.clear();

    for(int i = 0 ; i < maxdepth; ++i) {
        for(auto & nodeptr : curr_nodelist) {   
            brdptr_ = nodeptr;
            Perft(nodeptr->board_, 1);   

            //determine terminal state
            if(brdptr_->IsLeaf())
                brdptr_->terminal_ = true;
        }

        curr_nodelist = next_nodelist_;
        next_nodelist_.clear();
    }
  
    int total_after_branchsplit = 0;  

    //take each entry, put it in map, count up subnodes and store for in branchnode info. 
    for(auto & branch : root_->GetBranches()) { 
        int subtree_size = 0;  

        CountNodesAndLeafFrom(branch, subtree_size);
        branch->SetSubnodes(subtree_size);

        total_after_branchsplit += subtree_size;

        //assert we dont already have it(bug found right away!)
        m_assert((bool)map_divider_entries.count(branch->tag_) == false, "Position " + BoardAsFen(branch->board_) + " tries inserting " + branch->tag_ + " multiple times");
        
        map_divider_entries[branch->tag_] = branch;          
    }
    
    //std::cout << "Number of branches from root node : " << root->branches_.size() << std::endl;
    //std::cout << "Total nodes summing up all branches (non terminal), head node not counted : " << total_after_branchsplit << std::endl;
    return map_divider_entries;
}

void PerftDividerFactory::Perft(const Board & board, const int & depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth, true);
    else bmgen_.ParseLegalMoves(board, depth, true);
}

//promotion tags are 5 chars 
void PerftDividerFactory::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {   
    BoardNode* newNode = new BoardNode(b2);
    
    newNode->depth_ = current_depth_; // where am i using this?
    newNode->tag_ = info;

    std::transform(newNode->tag_.begin(), newNode->tag_.begin() + 4 , newNode->tag_.begin(), ::tolower); 
 
    brdptr_->InsertNode(newNode);

    next_nodelist_.push_back(newNode);   

}

void PerftDividerFactory::OnInsert(const Board& board, const int& depth) {  
    current_depth_ = depth;
}

