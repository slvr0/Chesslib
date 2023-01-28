#include "perft_divider.h"
#include "perft_mgfac.h"

//if node->next == null return n
//else return CountNodesAndLeafFrom(node->next, n + 1)


int PerftDividerFactory::CountNodesAndLeafFrom(BoardNode* node) {

    int counter = 0;

    if(node->IsLeaf()) {
        return 1;
    }
    
    for(auto & child : node->branches_) {
        counter += CountNodesAndLeafFrom(child);    

    }        

    return counter;
}

void PerftDividerFactory::Enumerate(const Board & board, const int & maxdepth) {    
    BoardNode* root = new BoardNode(board);

    std::vector<BoardNode*> curr_nodelist {root};
    next_nodelist_.clear();
    
    n = 1;

    for(int i = 0 ; i < maxdepth; ++i) {
        for(auto & nodeptr : curr_nodelist) {   
            brdptr_ = nodeptr;
            Perft(nodeptr->board_, 1);   
        }


        curr_nodelist = next_nodelist_;
        next_nodelist_.clear();
    }

    std::cout << "total : ";
    print(n);
    std::cout << "per branch : \n";

    int branch_total = 0;
    int total_after_branchsplit = 0;

    for(auto & branch : root->branches_) {
        branch_total = CountNodesAndLeafFrom(branch);
        total_after_branchsplit += branch_total;
        std::cout << branch->tag_ << " : " << branch_total << std::endl;
        
    }
    std::cout << "total summing up all branches : " << total_after_branchsplit << std::endl;
}

unsigned long PerftDividerFactory::Perft(const Board & board, const int & depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
    else bmgen_.ParseLegalMoves(board, depth);
}

void PerftDividerFactory::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {   
    BoardNode* newNode = new BoardNode(b2);
    
    newNode->tag_ = info.substr(0, 4);
    std::transform(newNode->tag_.begin(), newNode->tag_.begin() +4 , newNode->tag_.begin(), ::tolower); 
 
    brdptr_->InsertNode(newNode);

    next_nodelist_.push_back(newNode);    

    ++n; 

}

void PerftDividerFactory::OnInsert(const Board& board, const int& depth) {  
    
}

