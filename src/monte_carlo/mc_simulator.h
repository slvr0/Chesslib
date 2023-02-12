#pragma once

#include <memory>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_wmgenrollout.h"
#include "mc_bmgenrollout.h"
#include "mc_defs.h"

namespace MCTS {
class MCSimulator {
public :
    MCSimulator() {
        srand((unsigned) time(NULL));
    }

    GameResult SimulateGame(const Board& board);

private: 
    MCTS::WhiteRolloutMoveGenerator wrollout_mgen_;
    MCTS::BlackRolloutMoveGenerator brollout_mgen_;
};
}