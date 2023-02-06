#pragma once 

#include <thread>
#include <future>
#include <vector>

#include "move_generator.h"
#include "perft_mgfac.h"

//manager activates search threads , from root expands each branch into a MGThread [find a better way to do this]
//A MGThread is a perft with entry point Enumerate but that returns a future object holding a unsigned long long
//the run() command starts a asynch thread launch with its own context and this Enumerate entrypoint.

//simply collects all branches one depth down and returns
class MGBranchCollector : public MoveGeneratorHeader {
public: 
    MGBranchCollector() {

    }

    inline std::vector<Board> GetBranches(const Board& board) {
        branches_.clear();

        if(board.white_acts_) wmgen_.ParseLegalMoves(board, 1);
        else bmgen_.ParseLegalMoves(board, 1);  

        return branches_;
    }

    inline virtual void OnInsert(const Board& board, const int& depth) override {
        branches_.push_back(board);
    }

    //thinking of caching the size here... if this is used alot
    std::vector<Board> branches_;
};

struct SearchContent {
    unsigned long long n_entries = 0;
};

class MGThreadedEnumerator : public MoveGeneratorHeader{
public:
    MGThreadedEnumerator() {

    }

    inline void Search(const Board& board, const int& depth) {
        if(board.white_acts_) wmgen_.ParseLegalMoves(board, depth);
        else bmgen_.ParseLegalMoves(board, depth); 
    }

    inline virtual void OnInsert(const Board& board, const int& depth) override {
        ++n;
        if(depth < maxdepth_) {
            Search(board, depth); 
        }                 
    }

    unsigned long long Enumerate(const Board& board, const int& maxdepth) {
        maxdepth_ = maxdepth;

        Search(board, 1);
        return n;
    }

    std::future<unsigned long long> SpawnThread(const Board& board, const int& maxdepth) {     
        return std::async(std::launch::async, [this, board, maxdepth] {return Enumerate(board, maxdepth);});
    }

private: 
    unsigned long long n = 0 ;
    int maxdepth_ = 0;
    
};

class MGenThreadManager {
public:
    MGenThreadManager() {

    }

    unsigned long long Enumerate(const Board& board, const int & maxdepth) {
        MGBranchCollector branch_collector;

        std::vector<Board> branches = branch_collector.GetBranches(board);

        //now we want this result in a future object, and a thread for each entry

        std::vector<std::future<unsigned long long>>        thread_res;
        std::vector<std::unique_ptr<MGThreadedEnumerator>>  mg_threads_;
        
        for(int i = 0 ; i < branches.size(); ++i) {
            MGThreadedEnumerator* mg_thread = new MGThreadedEnumerator();
            const Board board = branches[i];
            thread_res.push_back(mg_thread->SpawnThread(board, maxdepth));
            mg_threads_.push_back(std::unique_ptr<MGThreadedEnumerator> (std::move(mg_thread)));
        }

        unsigned long long total_res = 0;
        for(auto & f_v : thread_res) {
            total_res += f_v.get();
        } 

        return total_res;
    }

private:
  
};