#pragma once

#include "../blowfish/chessboard.h"

namespace MCTS {

    //Random move generator result
    struct RMGResult {

        bool    nocheck_ = false;
        bool    terminal_ = false;
        Board   selected_;
        
        RMGResult() {
            
        }

        RMGResult(const bool& terminal, const bool& incheck, const Board& selected) : 
            terminal_(terminal), nocheck_(nocheck_), selected_(selected) {

        }
    };
}