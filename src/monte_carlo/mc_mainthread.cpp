#include "mc_mainthread.h"

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

    Node* nptr;

    int iter = 0;
    while(entries_++ < max_entries_) {  
                
        nptr = nodetree_->Reset();

        Timer t0;
        int expand_depth = 0;
        while(!nptr->IsLeaf()) {
            nptr = nptr->GetUpperConfidenceBranch();  
            ++nodelevel_searches_;                 
        }
        {
            const std::lock_guard<std::mutex> lock(expsim_lock);  
            if(!nodetree_->RequestNodeCheckIn(nptr)) {
                continue;
            }
        }
        
        ++expansions;
        delta_expansion += t0.elapsed();

        //attempt to acquire the nodelock
        //const std::lock_guard<std::mutex> lock(expsim_lock);            

        Timer t3;
        if(nptr->visits_ > 0) {             
            
            {
                const std::lock_guard<std::mutex> lock(expsim_lock);  
                expander_.Expand(nptr);
                entries_ += nptr->branches_.size();

                nodetree_->RequestNodeCheckOut(nptr);

                nptr = nptr->branches_[0]; //wops assert terminal first!  
                if(!nodetree_->RequestNodeCheckIn(nptr)) {
                    continue;   
                }
            }

        }

        const float res = simulator_.SimulateGame(nptr->board_);
        delta_simul += t3.elapsed();

        Timer t4;
        { //we lock, backpropagate result and give back the node           
            const std::lock_guard<std::mutex> lock(expsim_lock);
            nptr->Backpropagate(res);   
            nodetree_->RequestNodeCheckOut(nptr);  
        }
        delta_checkout += t4.elapsed();
    }

    std::cout << "Node level searches : " << nodelevel_searches_ << std::endl;
    std::cout << "Expansions (either simulation or grow tree) : " << expansions << std::endl;
    std::cout << "Average expansion depth : " << nodelevel_searches_ / expansions << std::endl;
    std::cout << "choose : " << delta_expansion << std::endl;
    std::cout << "checkin : " << delta_nodecheckin << std::endl;
    std::cout << "expand/simul :  " << delta_simul << std::endl;
    std::cout << "checkout : " << delta_checkout << std::endl;
    std::cout << "number of decisive games : " << simulator_.GetDecisiveGames() << std::endl;
    
}