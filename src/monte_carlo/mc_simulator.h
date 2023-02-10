#pragma once

#include <memory>

#include "../blowfish/chessboard.h"

#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

//creates threads for simulating random traversed chess games.
//we will adapt the move generator specifically for this as was always the intentions

struct SMGResult {
    const bool in_check_ = false;
    const bool terminal_ = false;

    Board* selected_move_ = nullptr;

    SMGResult(const bool& incheck, Board* select) : 
        in_check_(incheck), selected_move_(select) {

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

    inline const SMGResult MakeMove(const Board & board, const int & random_select) { 
        found_selected_ = false;
        terminal_ = true;

        random_select_ = random_select;    
        movecounter_ = 0;  

        bool incheck;
        if(board.white_acts_) incheck = wmgen_.ParseLegalMoves(board, 1);
        else incheck = bmgen_.ParseLegalMoves(board, 1);  
        
        if(terminal_) {
            return SMGResult(incheck, nullptr);
        }
        else {
            return SMGResult(incheck, found_selected_ ? select_.get() : any_.get());
        }
        
    }

    inline void OnInsert(const Board& board, const int& depth) override 
    { 
        ++n;           
        if(random_select_ >= movecounter_++) {
            *select_ = board;
            found_selected_ = true;
        }
        else {
            *any_ = board;
        }

        terminal_ = false;      
    }

private :
    int         random_select_;
    int         movecounter_;
    bool        found_selected_;
    bool        terminal_;

    std::unique_ptr<Board>  select_  = std::make_unique<Board> ();
    std::unique_ptr<Board>  any_     = std::make_unique<Board> ();
};


class MCSimulator {
public :
    MCSimulator() {
        srand((unsigned) time(NULL));
    }


    int Moves() const {
        return simulator_movegen_.n;
    }

    float SimulateGame(const Board& board);
    
    double generator_time_= 0;
private: 
    MCMGSimulatorMoveGenerator simulator_movegen_;
};