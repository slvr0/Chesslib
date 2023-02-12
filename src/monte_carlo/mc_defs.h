#pragma once

#include "../blowfish/chessboard.h"

namespace MCTS {

    //Random move generator result
    struct RMGResult {

        BBoard  nocheck_;
        bool    terminal_ = false;
        Board   selected_;
        
        RMGResult() {
            
        }

        RMGResult(const bool& terminal, const BBoard& nocheck, const Board& selected) : 
            terminal_(terminal), nocheck_(nocheck), selected_(selected) {

        }
    };

    //maybe this method of setting bounds can be integrated efficiently here 
    enum class  QualityFactorMethods {
    NodeVisits = 1, 
    NormalizedWinLooseRatio = 2
    };

    enum class TerminalState{ 
    NonTerminal = 0,
    Terminal    = 1,
    checkmate   = 2,
    Rule2       = 3
    };

    enum class GameResult {
    Undecisive = 0,
    WhiteWin   = 1, 
    BlackWin   = 2,
    Draw       = 3
    };
}