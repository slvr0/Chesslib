#pragma once

#include "../blowfish/chessboard.h"


//creates threads for simulating random traversed chess games.
//we will adapt the move generator specifically for this as was always the intentions

class MCSimulator {
public :
    MCSimulator() {

    }


    float SimulateGame(const Board& board) {
        return 1.f;
    }

private: 
    //MCRolloutGenerator !
};