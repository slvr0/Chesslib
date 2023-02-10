#include "mc_simulator.h"

#include "mc_functions.h"

float MCSimulator::SimulateGame(const Board &board)
{
    Board           select = board;
    MCTS::RMGResult res;
    float           score = 0.0f;

    //these should be config params later
    const int       random_selection_max = 25; // random number range, what happens if this is set low? certain moves like queen/castle will never be choosen
    const int       max_full_ply = 50;         // this is full move cycles (1 ply = 1 move white and 1 move black)
    const int       max_half_move = 50;        // without pushing a pawn

    while (select.full_move_ < max_full_ply)
    {
        //https://en.wiktionary.org/wiki/fifty-move_rule#:~:text=Proper%20noun&text=(chess)%20A%20rule%20in%20professional,invoked%20during%20an%20inconclusive%20endgame.
        if(!HasMatingMaterial(select) || (select.half_move_ >= max_half_move)) {
            score = 0.5f;
            break;
        }

        int random_select = rand() % random_selection_max;  

        res = select.white_acts_ ? wrollout_mgen_.ParseLegalMoves(select, random_select):
                                   brollout_mgen_.ParseLegalMoves(select, random_select);

        if(res.terminal_) {
            if(res.nocheck_) score  = 0.5f;        
            else score              = board.white_acts_ ^ select.white_acts_;                 
            break;
        }

        select = res.selected_;       
    }

    //debugging 
    if(score == 1) {
        ++n_decisive;
    }

    return score;
}
