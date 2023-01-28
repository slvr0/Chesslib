#include "perft_divider.h"
#include "perft_mgfac.h"

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

    print(n);
}

unsigned long PerftDividerFactory::Perft(const Board & board, const int & depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
    else bmgen_.ParseLegalMoves(board, depth);
}

void PerftDividerFactory::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {   


}

void PerftDividerFactory::OnInsert(const Board& board, const int& depth) {  
    BoardNode* newNode = new BoardNode(board);            
    brdptr_->InsertNode(newNode);

    next_nodelist_.push_back(newNode);    

    ++n;  
}


