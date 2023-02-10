#pragma once

#include <memory>

#include "../blowfish/chessboard.h"

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_wmgenrollout.h"
#include "mc_bmgenrollout.h"

//creates threads for simulating random traversed chess games.
//we will adapt the move generator specifically for this as was always the intentions

struct SMGResult {
    const bool  in_check_ = false;
    const bool  terminal_ = false;

    const Board selected_;

    SMGResult(const bool& terminal, const bool& incheck, const Board& selected) : 
        terminal_(terminal), in_check_(incheck), selected_(selected) {

    }
};


//if we bite the dust on performance , the move generator for this has to be specialized , we could potentially unlock much more efficiency
class MCMGSimulatorMoveGenerator : public MoveGeneratorHeader {
public: 

    int n = 0 ;

    MCMGSimulatorMoveGenerator() {

    }

    ~MCMGSimulatorMoveGenerator() {
 
    }   

    double movetime_parsing = 0;
    double insertion_asserting = 0;    

    inline const SMGResult MakeMove(const Board & board, const int & random_select) { 
        Timer t0;

        found_selected_     = false;
        terminal_           = true;

        random_select_      = random_select;    
        movecounter_        = 0;  

        bool                incheck;

        if(board.white_acts_) incheck = wmgen_.ParseLegalMoves(board, 1);
        else incheck = bmgen_.ParseLegalMoves(board, 1);  

        movetime_parsing += t0.elapsed();
        
        if(terminal_) {
            return SMGResult(terminal_, incheck, board);
        }
        else {
            return SMGResult(terminal_, incheck, found_selected_ ? select_ : any_);
        }
        
    }

    inline void OnInsert(const Board& board, const int& depth) override 
    { 
        Timer t1;
        ++n;           
        if(random_select_ >= movecounter_++) {
            select_ = board;
            found_selected_ = true;
        }
        else {
            any_ = board;
        }

        terminal_ = false;  

        insertion_asserting += t1.elapsed();    
    }

private :
    int         random_select_;
    int         movecounter_;
    bool        found_selected_;
    bool        terminal_;

    Board select_;
    Board any_; //any is maybe not the right expression, last / current is more appropriate
};


class MCSimulator {
public :
    MCSimulator() {
        srand((unsigned) time(NULL));
    }

    int Moves() const {
        return simulator_movegen_.n;
    }

    float MoveTime() const {
        return simulator_movegen_.movetime_parsing;
    }

    float InsertAssert() const {
        return simulator_movegen_.insertion_asserting;
    }

    float SimulateGame(const Board& board);
    
    double generator_time_= 0;
    double rng_timer = 0;   
    int n_decisive = 0;
    

private: 
    MCMGSimulatorMoveGenerator  simulator_movegen_;

    MCTS::WhiteRolloutMoveGenerator wrollout_mgen_;
    MCTS::BlackRolloutMoveGenerator brollout_mgen_;
};