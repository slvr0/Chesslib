#include "mc_simulator.h"

#include <random>

#include "mc_functions.h"

using namespace MCTS;

GameResult MCSimulator::SimulateGame(const Board &board)
{
    Board           select = board;
    MCTS::RMGResult res; 

    //these should be config params later
    const int       random_selection_max = 50; // random number range, what happens if this is set low? certain moves like queen/castle will never be choosen
    const int       max_full_ply = 300;         // this is full move cycles (1 ply = 1 move white and 1 move black)
    const int       max_half_move = 50;        // without pushing a pawn
    
    int round = 0;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    
    std::minstd_rand0 gen (seed);

    auto linear_decayingdistr = [round]() { return std::clamp((int)(60 - .5*round), 0, 25);};

    std::poisson_distribution<int>              distribution1(random_selection_max/5);
    std::chi_squared_distribution<double>       distribution2(random_selection_max/4);
    std::normal_distribution<double>            distribution3(random_selection_max/6, 4);
    std::gamma_distribution<double>             distribution4(random_selection_max/6, 4);


    while (round++ < max_full_ply)
    {
        //https://en.wiktionary.org/wiki/fifty-move_rule#:~:text=Proper%20noun&text=(chess)%20A%20rule%20in%20professional,invoked%20during%20an%20inconclusive%20endgame.
        if(!HasMatingMaterial(select) || (select.half_move_ >= max_half_move)) {                       
            return GameResult::Draw;
        }

        //int random_select = random_selection_max - rand() % random_selection_max;  

        int rmethod = rand() % 4;     
        int random_select;
        switch(rmethod) {
        case 0 : random_select = distribution1(gen); break;
        case 1 : random_select = distribution2(gen); break;
        case 2 : random_select = distribution3(gen); break;
        case 3 : random_select = distribution4(gen); break;
        case 4 : random_select = rand() % random_selection_max; break;
        }

      


        res = select.white_acts_ ? wrollout_mgen_.ParseLegalMoves(select, random_select):
                                   brollout_mgen_.ParseLegalMoves(select, random_select);

        if(res.terminal_) {
            if(res.nocheck_) { 
                return GameResult::Draw;                    
            } else { 
                return res.selected_.white_acts_ ? GameResult::BlackWin : GameResult::WhiteWin;
            }             
        }

        select = res.selected_;       
    }

    return GameResult::Undecisive;
}

GameResult MCSimulator::SimulateGameOld(const Board &board)
{
    Board           select = board;
    MCTS::RMGResult res; 

    //these should be config params later
    const int       random_selection_max = 16; // random number range, what happens if this is set low? certain moves like queen/castle will never be choosen
    const int       max_full_ply = 300;         // this is full move cycles (1 ply = 1 move white and 1 move black)
    const int       max_half_move = 50;        // without pushing a pawn
    
    int round = 0;

    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::minstd_rand0 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    
 
    std::poisson_distribution<int>              distribution1(random_selection_max/5);
    std::chi_squared_distribution<double>       distribution2(random_selection_max/4);
    std::normal_distribution<double>            distribution3(random_selection_max/6, 4);
    std::gamma_distribution<double>             distribution4(random_selection_max/6, 4);

    while (round++ < max_full_ply)
    {
        //https://en.wiktionary.org/wiki/fifty-move_rule#:~:text=Proper%20noun&text=(chess)%20A%20rule%20in%20professional,invoked%20during%20an%20inconclusive%20endgame.
        if(!HasMatingMaterial(select) || (select.half_move_ >= max_half_move)) {                       
            return GameResult::Draw;
        }

        if(select.white_acts_) ++nwhites;
        else ++nblacks;

        //int random_select = rand() % random_selection_max;  

        int rmethod = 4;     
        int random_select;
        switch(rmethod) {
            case 0 : random_select = distribution1(gen); break;
            case 1 : random_select = distribution2(gen); break;
            case 2 : random_select = distribution3(gen); break;
            case 3 : random_select = distribution4(gen); break;
            case 4 : random_select = rand() % random_selection_max; break;
        }

 
   
        randomnrtotal += random_select;
        if(select.white_acts_) rwhite += random_select;
        else rblack += random_select;
       
        ++ngens;

        SMGResult res = sim_.MakeMove(select, random_select);
        
        

        if(res.terminal_) {
            if(res.nocheck_) { 
                return GameResult::Draw;                    
            } else { 
                return select.white_acts_ ? GameResult::BlackWin : GameResult::WhiteWin;
            }             
        }
        if(random_select >= res.movelist_.size()) random_select = rand() % res.movelist_.size();
      

        select = res.movelist_[random_select];

          
    }

    return GameResult::Undecisive;
}

void MCSimulator::SetNodeMetaData(Node* node) {
    const Board board = node->board_;

    MCTS::RMGResult res = board.white_acts_ ? wrollout_mgen_.ParseLegalMoves(board, 0):
                                   brollout_mgen_.ParseLegalMoves(board, 0);

    if(res.terminal_) {
        if(res.nocheck_) { 
            node->SetTerminal(MCTS::TerminalState::StaleMateRule2);                
        } else { 
            node->SetTerminal(MCTS::TerminalState::Terminal);
        } 
    }
    else node->SetTerminal(MCTS::TerminalState::NonTerminal);

    //metadata has been acknowledged, can contribute to a Q validation
    node->SetSolid();

    return;

    //Set bounds here later when it becomes important
}
