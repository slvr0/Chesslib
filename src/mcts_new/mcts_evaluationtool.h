#ifndef MCTS_EVALUATIONTOOL_H
#define MCTS_EVALUATIONTOOL_H

#include <vector>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/wmgen_randselect.h"
#include "../blowfish/bmgen_randselect.h"

#include "mcts_defines.h"

namespace MCTS_SIM {
    typedef std::vector<Board> MoveList;

    FORCEINL bool HasMatingMaterial(const Board& board) {
    if( (board.white_rook_  | board.black_rook_) ||
        (board.white_pawn_  | board.black_pawn_) ||
        (board.white_queen_ | board.black_queen_) ) return true;

    if(BitCount(board.white_ | board.black_) < 4 ) return false;

    if(board.white_knight_ | board.black_knight_) return true;

    BBoard lq_bishop = 0x55AA55AA55AA55AAULL;
    BBoard dq_bishop = 0xAA55AA55AA55AA55ULL;

    BBoard lq_at = (board.white_bishop_ | board.black_bishop_) & lq_bishop;
    BBoard dq_at = (board.white_bishop_ | board.black_bishop_) & dq_bishop; 

    return lq_at || dq_at;
    }

class MCTSSimulationStateGenerator : public MoveGeneratorHeader {
public:
    MCTSSimulationStateGenerator();

    
    std::pair<PositionStatus, Board> GetTransitions(const Board& board, const int& select); //possible moves, and if we're under check

    void OnInsert(const Board& board, const int& depth) override;

private:
    Board   selected_;
    Board   any_;
    int     rselect_;   
    int     entries_;  
};

//updates nodemodel metrics based on DNN input or random simulation 
class MCTSModelEvaluation {
public:
    MCTSModelEvaluation(); 
    ~MCTSModelEvaluation() {
    }

    SimulationResult                    SimulateGameplay(const Board& board, const OptionsDict& params);
    std::pair<PositionStatus, Board>    GenerateStochasticTransition(const Board& board, const int& rselect);
    

private:
    MCTSSimulationStateGenerator state_generator_;
    WhiteRolloutMoveGenerator    white_state_generator_;
    BlackRolloutMoveGenerator    black_state_generator_;
};
}

#endif