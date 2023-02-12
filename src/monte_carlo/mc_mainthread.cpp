#include "mc_mainthread.h"
#include "mc_config.h"

// https://ai.stackexchange.com/questions/2637/what-should-we-do-when-the-selection-step-selects-a-terminal-state
// states behaviour in case of new expanded state is terminal 

using namespace MCTS;

std::mutex expsim_lock;

//since rollouts are the major bottleneck of the system, it needs to be considered to have a thread que for this
bool MCExpandSimulateThread::Ponder() {
    m_assert(nodetree_, "node tree structure is not set in thread");

    int n = 0;

    int nodelevel_searches_ = 0;
    int expansions = 0;
    int maxdepth = 0;
    int entries = 0;
    const int nmax_searches = MCTS::MCConfigurationParams::nmax_threadsearches;
    
    Node* current;

    int iter = 0;

    while(entries < nmax_searches) {
        current = nodetree_->Reset();
        while(!current->IsLeaf()) current = current->GetBestPNode();

/*         ++iter;
        print(current->Debug());
        if(iter == 20) exit(1); */

        if(current->N_ > 0) {
            expander_.Expand(current);
            entries += current->edges_.size();
            if(current->IsLeaf()) { current->MakeTerminal(TerminalState::Terminal); continue;} // how do set which terminal state we're in, terminal should influence policy somehow
            current = current->edges_[0];           
        } 

        GameResult result = simulator_.SimulateGame(current->board_);

        current->Backpropagate(result);  
               
       
    }





























}



/*     
    std::cout << "------THREAD INFO------" << std::endl;
    std::cout << "Entries : " << entries_ << std::endl;
    std::cout << "Node level searches : " << nodelevel_searches_ << std::endl;
    std::cout << "Expansions (either simulation or grow tree) : " << expansions << std::endl;
    std::cout << "Average expansion depth : " << nodelevel_searches_ / expansions << std::endl;
    std::cout << "choose : " << delta_expansion << std::endl;
    std::cout << "checkin : " << delta_nodecheckin << std::endl;
    std::cout << "expand/simul :  " << delta_simul << std::endl;
    std::cout << "checkout : " << delta_checkout << std::endl;
    std::cout << "number of decisive games : " << simulator_.GetDecisiveGames() << std::endl;
    std::cout << "number of times trying to access occupied node : " << num_ignores << std::endl;
    std::cout << "max search depth : " << maxdepth << std::endl;
    std::cout << "Root visits : "  << nodetree_->Reset()->visits_ << std::endl;
    std::cout << "------------------------" << std::endl;
  
}

   */













































/* 

    Node* nptr;

    int iter = 0;
    while(entries_++ < nmax_searches) {  
                
        nptr = nodetree_->Reset();

        Timer t0;
        int expand_depth = 0;
        {
            const std::lock_guard<std::mutex> lock(expsim_lock); 
            while(!nptr->IsLeaf()) {
                nptr = nptr->GetUpperConfidenceBranch();  
                ++nodelevel_searches_;  
                ++expand_depth;               
            }
        }

        {
            const std::lock_guard<std::mutex> lock(expsim_lock);  
            if(!nodetree_->RequestNodeCheckIn(nptr)) {
                ++num_ignores;
                continue;
            }
        }
        
        if(expand_depth > maxdepth) maxdepth = expand_depth;

        ++expansions;
        delta_expansion += t0.elapsed();

        //attempt to acquire the nodelock
        //const std::lock_guard<std::mutex> lock(expsim_lock);            

        Timer t3;
        if(nptr->N_ > 0) { 
            {
                const std::lock_guard<std::mutex> lock(expsim_lock);  
                expander_.Expand(nptr);
                entries_ += nptr->branches_.size();

                nodetree_->RequestNodeCheckOut(nptr);

                if(nptr->IsLeaf()) continue;
                else nptr = nptr->branches_[0]; //wops assert terminal first!  

                if(!nodetree_->RequestNodeCheckIn(nptr)) {
                    ++num_ignores;
                    continue;   
                }
            }

        } */