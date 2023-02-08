#include "mc_mainthread.h"

// https://ai.stackexchange.com/questions/2637/what-should-we-do-when-the-selection-step-selects-a-terminal-state
// states behaviour in case of new expanded state is terminal 

using namespace MCTS;

std::mutex expsim_lock;

bool MCExpandSimulateThread::Ponder() {
    m_assert(nodetree_, "node tree structure is not set in thread");

    while(entries_++ < max_entries_) {        
        Node* nptr = nodetree_->Reset();

        while(!nptr->IsLeaf()) {
            nptr = nptr->GetUpperConfidenceBranch();            
        }

        { //attempt to acquire the nodelock
            const std::lock_guard<std::mutex> lock(expsim_lock);            
            if(!nodetree_->RequestNodeCheckIn(nptr)) {                
                continue;
            }
        }

        if(nptr->visits_ > 0) {   
               
            expander_.Expand(nptr);
            nptr = nptr->branches_[0]; //wops assert terminal first!
        }

        const float res = simulator_.SimulateGame(nptr->board_);

        { //we lock, backpropagate result and give back the node           
            const std::lock_guard<std::mutex> lock(expsim_lock);
            nptr->Backpropagate(res);
            nodetree_->RequestNodeCheckOut(nptr);
        }
    }
}