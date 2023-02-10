#include "mc_simulator.h"

float MCSimulator::SimulateGame(const Board& board) {
    float result = 0.f;

    const int random_selection_max = 35; //random number range, what happens if this is set low? certain moves like queen/castle will never be choosen
    const int max_full_ply = 100; //this is full move cycles (1 ply = 1 move white and 1 move black)
    const int max_half_move = 50; //without pushing a pawn

    std::unique_ptr<Board> bptr = std::make_unique<Board> (board);

    int round = board.full_move_;



    while(round++ < max_full_ply) {
        int random_select = rand() % random_selection_max;

        Timer t0;
        SMGResult res = simulator_movegen_.MakeMove(*bptr, random_select);
        generator_time_ += t0.elapsed();

        if(res.selected_move_) {
            //*bptr.get() = (*res.selected_move_);
            *bptr = *res.selected_move_;
         
        }
        else { //no legal moves, either stalemate or checkmate
            
        }

        

    }


    













    return result;
}