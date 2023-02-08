#pragma once

#include <future>

#include "mc_mainthread.h"
#include "mc_node_structure.h"

//creates and manages the threads for insertion/expansion and simulation/backpropagation.

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
            thread_evals_.push_back(main_thread->SpawnThread());
            main_threads_.push_back(main_thread);
        }
    }
/* 
    bool CreateExpandSimulateThread()
            std::vector<std::future<unsigned long long>>        thread_res;
        std::vector<std::unique_ptr<MGThreadedEnumerator>>  mg_threads_;
        
        for(int i = 0 ; i < branches.size(); ++i) {
            MGThreadedEnumerator* mg_thread = new MGThreadedEnumerator();
            const Board board = branches[i];
            thread_res.push_back(mg_thread->SpawnThread(board, maxdepth));
            mg_threads_.push_back(std::unique_ptr<MGThreadedEnumerator> (std::move(mg_thread)));
        } */

private:
    std::vector<MCExpandSimulateThread*> main_threads_;
    std::vector<std::future<bool>> thread_evals_; //placeholder_1???
    NodeTreeStructure* nodetree_;
    
    
};
}