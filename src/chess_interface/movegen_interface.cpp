#include "movegen_interface.h"


#include "../blowfish/move_generator.h"
#include "../blowfish/perft_mgfac.h"
#include "../blowfish/perft_divider.h"

std::vector<int> ChesslibInterface::InitSearch(const Board &board, SearchType search_type, int maxdepth){
    std::vector<int> search_result;

    switch(search_type) {
        case SearchType::PERFT : {
            PerftMGFactory perft_factory_;            
            search_result = perft_factory_.Enumerate(board, maxdepth);  
            return search_result;        }

        case SearchType::PERFT_DIVIDE : {
            PerftDividerFactory perft_divider_factory;            
            perft_divider_factory.Enumerate(board, maxdepth);  
            return search_result;
        }
        
    }
    

}
