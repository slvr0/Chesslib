#ifndef MCTS_JOBQUEUE_H
#define MCTS_JOBQUEUE_H

#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>
#include <functional>
#include <future>
#include <vector>

#include "mcts_nodetree.h"
#include "mcts_simulation_env.h"

class Worker {
public: 
   Worker() { 

   }

   std::future<MCTSNodeTree*> SpawnThread(MCTSNodeTree* branch) {     
      return std::async(std::launch::async, [this, branch] {return this->Delegate(branch);});
   }

   MCTSNodeTree* Delegate(MCTSNodeTree* branch) {
      MCTSSimulationEnvironment simenv;
      OptionsDict params;
      simenv.Search(branch, params);
      return branch;
   }

};

class ThreadPool {

public:
   ThreadPool() {

   }

   MCTSNodeTree* AddJob(MCTSNodeTree* orig, std::vector<MCTSNodeTree*> threadbranches) {
      for(auto & branch : threadbranches) {
         Worker w;
         threadreturns_.push_back(w.SpawnThread(branch));
         threads_.emplace_back(w);
      }

      for(auto & f_v : threadreturns_) {
         orig->AttachSubTree(f_v.get(), true);
      } 
   }

   std::vector<std::future<MCTSNodeTree*>> threadreturns_;
   std::vector<Worker> threads_;

};




#endif