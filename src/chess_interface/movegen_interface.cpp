#include "movegen_interface.h"



int ChesslibInterface::InitSearch(const Board &board, SearchType search_type, int maxdepth){
    switch(search_type) {
        case SearchType::PERFT : {
            PerftMGFactory perft_factory_;            
            int N = perft_factory_.Enumerate(board, maxdepth);  
            return N;
        }

        case SearchType::PERFT_DIVIDE : {
            PerftDividerFactory perft_divider_factory;            
            perft_divider_factory.Enumerate(board, maxdepth);  
            return 0;
        }
    }

    return 0;
}
