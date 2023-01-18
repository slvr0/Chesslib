#include "perft_divider.h"
#include "perft_mgfac.h"

void PerftDividerFactory::Enumerate(const Board & board, const int & maxdepth) {
    max_depth_ = maxdepth;
    int _ = Perft(board, 1);

    PerftMGFactory perft_mg;

    for(auto const& entry : div_entries_) {
        std::string hash = entry.first;
        DividerContextObject* context = entry.second;
        context->entries = perft_mg.Enumerate(context->board_entry, maxdepth - 1);

        std::cout << hash << "\t : \t" << context->entries << std::endl;
    }
}

unsigned long PerftDividerFactory::Perft(const Board & board, const int & depth) {
    if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
    else bmgen_.ParseLegalMoves(board, depth);
}

void PerftDividerFactory::OnInsertDebug(const Board& b1, const Board& b2, const std::string & info) {   

    std::string cutn_dry = info.substr(0, 4);
    std::transform(cutn_dry.begin(), cutn_dry.end(), cutn_dry.begin(), ::tolower); 
    div_entries_[cutn_dry] = new DividerContextObject(b2);
}

void PerftDividerFactory::OnInsert(const Board& board, const int& depth) {     
    //not necessary in head, adding map entries in insert debug
}