#pragma once

#include <memory>

#include "../blowfish/chessboard.h"
#include "../blowfish/move_generator.h"
#include "../blowfish/defs.h"

#include "mc_wmgenrollout.h"
#include "mc_bmgenrollout.h"
#include "mc_defs.h"
#include "mc_node.h"

namespace MCTS {

struct SMGResult {
    const bool  nocheck_ = false;
    const bool  terminal_ = false;

    std::vector<Board> movelist_;
    

    SMGResult(const bool& terminal, const bool& nocheck, std::vector<Board> movelist) : 
        terminal_(terminal), nocheck_(nocheck_), movelist_(movelist) {

    }
};


//if we bite the dust on performance , the move generator for this has to be specialized , we could potentially unlock much more efficiency
// oh we bite the dust alright
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
        blist_.clear();

        bool                nocheck;

        if(board.white_acts_) nocheck = wmgen_.ParseLegalMoves(board, 1);
        else nocheck = bmgen_.ParseLegalMoves(board, 1);  

        movetime_parsing += t0.elapsed();
        
    
        return SMGResult(terminal_, nocheck, blist_);
       
   
        
    }

    inline void OnInsert(const Board& board, const int& depth) override 
    { 
        Timer t1;
        ++n;     

        blist_.push_back(board);
        /*       
        if(random_select_ == movecounter_++) {
            select_ = board;
            found_selected_ = true;
        }
        else {
            any_ = board;
        }
 */
        terminal_ = false;  

        insertion_asserting += t1.elapsed();    
    }

private :
    int         random_select_;
    int         movecounter_;
    bool        found_selected_;
    bool        terminal_;
    std::vector<Board> blist_;
    Board select_;
    Board any_; //any is maybe not the right expression, last / current is more appropriate
};


class MCSimulator {
public :
    MCSimulator() {
        srand((unsigned) time(NULL));
        nwhites = 0;
        nblacks = 0;
        randomnrtotal = 0;
        rwhite = 0;
        rblack = 0;
        ngens = 0;
        
    }

    GameResult SimulateGame(const Board& board);
    GameResult SimulateGameOld(const Board &board);

    //this should not happen here
    void SetNodeMetaData(Node* node);

    int nwhites = 0;
    int nblacks = 0;
    int randomnrtotal = 0;
    int rwhite = 0;
    int rblack = 0;
    int ngens = 0;

private: 
    MCTS::BlackRolloutMoveGenerator brollout_mgen_;
    MCTS::WhiteRolloutMoveGenerator wrollout_mgen_;
   

    MCMGSimulatorMoveGenerator sim_;


};
}