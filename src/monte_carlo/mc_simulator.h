#pragma once

#include <memory>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_wmgenrollout.h"
#include "mc_bmgenrollout.h"

class MCSimulator {
public :
    MCSimulator() {
        srand((unsigned) time(NULL));
    }

    float SimulateGame(const Board& board);

    int GetDecisiveGames() const { 
        return n_decisive;
    }

private: 
    MCTS::WhiteRolloutMoveGenerator wrollout_mgen_;
    MCTS::BlackRolloutMoveGenerator brollout_mgen_;

    int n_decisive = 0;
};