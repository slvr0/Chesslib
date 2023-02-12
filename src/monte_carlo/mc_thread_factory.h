#pragma once

#include <future>

#include "mc_mainthread.h"
#include "mc_node_structure.h"

//creates and manages the threads for insertion/expansion and simulation/backpropagation.


//another idea is to Start a thread on each branch and they return the root score , 
//this would solve alot of concurrency issues (we'd split the tree completly)
//the more i think about this idea i realize its superior


namespace MCTS {
class MCThreadFactory {
public:  
    MCThreadFactory(NodeTreeStructure* nodetree) : 
        nodetree_(nodetree) {
        
    }

    //remake this later to have better control
    bool SpawnThreads(const size_t& nthreads) {
        for(int i = 0 ; i < nthreads ; ++i ) {
            MCExpandSimulateThread* main_thread = new MCExpandSimulateThread(nodetree_);
            thread_evals_.push_back(main_thread->Start());
            main_threads_.push_back(main_thread);
        }    

        for(auto & future_thread: thread_evals_) {
            future_thread.wait();
        }
        nodetree_->Evaluate();   
    }

private:
    std::vector<MCExpandSimulateThread*> main_threads_;
    std::vector<std::future<bool>> thread_evals_; //placeholder_1???
    NodeTreeStructure* nodetree_;
    
};
}