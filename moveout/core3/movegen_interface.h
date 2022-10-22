#pragma once

#include "chesslib_scenarios.h"


//this as a movegen callback when a legal move is created , depending on the search environment

/*
1. in Perft = recursevily requests a new full search for each legal move, with a counter
2. in MCTS node discovery : attaches a branch node for each found legal move 
3. in MCTS rollout mode : keeps a counter for each found move, when it matches a random generated number , it updates the board status from the move and traverses
4. in Debug 1D mode : makes a printout for each move found
*/

namespace Chesslib2 {

enum DiscoveryMode {
    PERFT = 1 , 
    MCTS_DISCOVERY = 2, 
    MCTS_ROLLUOT = 3,
    DEBUG = 4
};


class ChessMoveGeneratorInterface {

public : 

void CallbackMoveFound() {

}

void initSearch(DiscoveryMode mode) {
    
}

private:
    DiscoveryMode discovery_mode;
};

}; //eof namespace